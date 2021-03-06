#ifndef LINKS_H
#define LINKS_H

#include<MySqlService.h>
#include <QObject>

class LINKS
{
public:
    static QString APILINK;
  //  explicit LINKS(QObject *parent = nullptr);
static QString getProfilePicture(QString username)
{
   return APILINK+"/upload/"+username+".jpeg";
}
static QString getLocationPicture(int idLokacije, int idSlike)
{
    MyQuery str("%1/upload/LOK%2SLIKA%3.jpeg");
    str<<APILINK<<idLokacije<<idSlike;
    return str.toStr();
}

static QString getProfileDefaultPicture()
{
    return APILINK+"/upload/noavailableprofilepicture.jpeg";
}


static QString getNoImageAvailable()
{
    return APILINK+"/upload/noimageavailable.jpeg";
}

signals:

};

#endif // LINKS_H
