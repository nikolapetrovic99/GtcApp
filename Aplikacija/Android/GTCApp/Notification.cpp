#include "Notification.h"
Notification* Notification::instance = NULL;
int Notification::NewFriendNotification = 0;
int Notification::NewShareNotification = 0;
QNetworkAccessManager* Notification::manager = NULL;

Notification::Notification(){
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Notification::AsyncNotification));
    timer->start(5000);

    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished, this, &Notification::ReplyNotificationData);
}

void Notification::SendRequest(QString query)
{
    QUrlQuery params;
    params.addQueryItem("query", query);
    params.addQueryItem("querytype","aync");

    QUrl ur(LINKS::APILINK+"/Database/GtcService.php");
    QNetworkRequest req(ur);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    manager->post(req, params.query().toUtf8());
}

void Notification::ReplyNotificationData(QNetworkReply *reply)
{
    MySqlTable t;
    t = reply->readAll();
    if(t.isSuccessfully())
    {
        if(LOCALDATA::ulogovan==true)
        {
            if(NewFriendNotification <  t.Rows[0]["Zahtevi"].toInt())
            {
                sendPushNotification("Imate novi zahtev za prijateljstvo!","Zahtev za prijateljstvo");
            }
            if(NewShareNotification <  t.Rows[0]["Obavestenja"].toInt())
            {
                sendPushNotification("Imate novu poruku u sanducetu!","Nova poruka");
            }

            NewFriendNotification = t.Rows[0]["Zahtevi"].toInt();
            NewShareNotification = t.Rows[0]["Obavestenja"].toInt();
            //emit GetInstance().DataChanged();
            qDebug()<<"ZAHTEVI"<<t.Rows[0]["Zahtevi"];
            emit GetInstance().updateFriendNotification(NewFriendNotification);
            emit GetInstance().updateSharedNotification(NewShareNotification);
        }

        emit MySqlService::MySqlInstance().myConnectionEstablished();
        //   qDebug()<<"Broj notifikacija: "<<NewNotification;
    }
    else
    {
        emit MySqlService::MySqlInstance().myConnectionLost();
    }
}

void Notification::AsyncNotification(){
    MyQuery query;
    query="SELECT Obavestenja,Zahtevi from(SELECT COUNT(*) as Obavestenja From Obavestenje WHERE reciever='%1' and vidjen = 0) as t1, (SELECT count(*) as Zahtevi FROM `Zahtev` WHERE primalac='%1' and vidjen='0') as t2";
    query<<LOCALDATA::mProfil->getKorisnickoIme();
    SendRequest(query.toStr());
}

Notification &Notification::GetInstance()
{
    if(instance ==NULL)
        instance = new Notification();

    return *instance;
}

void Notification::ucitajZahtevi()
{
    MySqlService &s = MySqlService::MySqlInstance();

    MySqlTable t;
    MyQuery query;
    query="SELECT Korisnik.*,Zahtev.vidjen FROM `Zahtev` INNER join Korisnik on Zahtev.posiljalac=Korisnik.korisnickoIme WHERE primalac='%1' order by vidjen asc";
    query<<LOCALDATA::mProfil->getKorisnickoIme();

    t =s.WSendQuery(query);
    if(t.isSuccessfully())
    {
        ZahteviModel &zm=ZahteviModel::GetInstance();
        zm.removeAll();
        for(int i =0;i<t.Count();i++)
        {
            zm.dodajzahtev(zahtev((t.Rows[i]["slika"].toInt() == 0? LINKS::getProfileDefaultPicture():LINKS::getProfilePicture(t.Rows[i]["korisnickoIme"])),t.Rows[i]["ime"]+" "+t.Rows[i]["prezime"],t.Rows[i]["korisnickoIme"],t.Rows[i]["vidjen"].toInt(),false));
        }
        zm.setVidjeni();
    }

}

int Notification::getBrNovihZahteva()
{
    return NewFriendNotification;
}

void Notification::podeliLokaciju(int id, QString username)
{
    deljenje(id,username,0);
}

void Notification::podeliDogadjaj(int id, QString username)
{
    deljenje(id,username,1);
}

void Notification::deljenje(int id, QString username, int tip)
{
    MySqlService &s = MySqlService::MySqlInstance();
    MyQuery query;
    query="INSERT INTO Obavestenje(`sender`, `reciever`, `tip`, `idDL`) VALUES('%1','%2',%3,%4)";
    query<<LOCALDATA::mProfil->getKorisnickoIme()<<username<<tip<<id;
    s.SendQuery(query);
    qDebug()<<"podeljeno" << query.toStr();
}

void Notification::setVidjeni(int id)
{
    MySqlService &s = MySqlService::MySqlInstance();
    MyQuery query;
    query="UPDATE Obavestenje SET vidjen=1 WHERE idObavestenja=%1";
    query<<id;
    s.SendQuery(query);
    //  qDebug()<<"sdsdsaas "<<query.toStr();
}

void Notification::ucitajObavestenja()
{
    MySqlService &s = MySqlService::MySqlInstance();
    MySqlTable t;
    MyQuery query;
    query="SELECT Obavestenje.idDL,Obavestenje.tip,Obavestenje.vidjen,Obavestenje.idObavestenja,Korisnik.ime,Korisnik.prezime,Korisnik.korisnickoIme,Korisnik.slika FROM `Obavestenje` INNER join Korisnik on Obavestenje.sender=Korisnik.korisnickoIme WHERE reciever='%1' ORDER BY idObavestenja DESC";
    query<<LOCALDATA::mProfil->getKorisnickoIme();
    t =s.WSendQuery(query);
    if(t.isSuccessfully())
    {
        ObavestenjaModel &om=ObavestenjaModel::GetInstance();
        om.removeAll();
        QString tekst;
        for(int i =0;i<t.Count();i++)
        {
            ////ovde se treba izmeni
            tekst="Korisnik "+t.Rows[i]["ime"]+" "+t.Rows[i]["prezime"]+" deli " +(t.Rows[i]["tip"].toInt() == 0?"lokaciju":"dogadjaj");
            om.dodajobavestenje(obavestenje((t.Rows[i]["slika"].toInt() == 0? LINKS::getProfileDefaultPicture():LINKS::getProfilePicture(t.Rows[i]["korisnickoIme"])),tekst,t.Rows[i]["korisnickoIme"],false,t.Rows[i]["tip"].toInt(),t.Rows[i]["idDL"].toInt(),t.Rows[i]["vidjen"].toInt(),t.Rows[i]["idObavestenja"].toInt()));
        }
    }

}

void Notification::sendPushNotification(QString text, QString naslov)
{
    QtAndroid::androidContext().object();
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(text);
    QAndroidJniObject javaTitle = QAndroidJniObject::fromString(naslov);

    // QAndroidJniObject::callStaticMethod<void>("com/notification/javalib/NotificationClient","notify");

    QAndroidJniObject::callStaticMethod<void>(
                "com/notification/javalib/NotificationClient",
                "notify",
                "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V",
                QtAndroid::androidContext().object(),
                javaNotification.object<jstring>(),
                javaTitle.object<jstring>()
                );
}
