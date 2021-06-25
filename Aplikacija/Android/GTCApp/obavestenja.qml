import QtQuick 2.0
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
Rectangle
{
   anchors.fill: parent
   Text
   {
       id: obavestenjatekst
       text: qsTr("Obavestenja")
       font.family: "Helvetica"
       font.pointSize: 30
       anchors.horizontalCenter: parent.horizontalCenter
   }
   Rectangle
   {
       id:linijica
       width: parent.width
       height: 1
       color: "#c9c9c9"
       anchors.top: obavestenjatekst.bottom
       anchors.topMargin:10
   }


   ScrollView
   {
       id:scrolobavestenja
       anchors.top: linijica.bottom
       width: parent.width
       height: parent.height

       ListView
       {
            boundsBehavior: ListView.StopAtBounds
            clip:true
            anchors.fill: parent
            width: parent.width
            height: parent.height
            model:_obavestenjamodel
            delegate: ItemDelegate
            {
                height:60
                width:scrolobavestenja.width
               required property int id
               required property string slika
               required property string tekst
               required property int index
               required property bool cekirano
               required property bool lid
               Rectangle
               {
                   anchors.fill: parent
                   Component.onCompleted:
                   {
                       if(cekirano==true)
                       {
                           color="white"
                       }
                       else
                       {
                          color="#b8dff2"
                       }
                   }
                   Rectangle
                   {

                   id:glavnirect
                   width: parent.width
                   height: parent.height-10
                   anchors.verticalCenter: parent.verticalCenter
                    color: "transparent"
                    Image {
                        id: obrisiobavestenje
                        source: "qrc:/new/prefix1/close-removebg-preview.png"
                        height:8
                        width: 8
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.rightMargin: 2
                        MouseArea
                        {
                            z: 100
                            anchors.fill: parent
                            onClicked:
                            {

                                  console.log(index)
                                _obavestenjamodel.obrisi(index)

                            }
                        }
                    }
                   Rectangle
                   {

                       //border.color:"#595959"
                       color: "transparent"
                       id:slikarect
                       height: parent.height
                       width: parent.height
                       anchors.left: parent.left
                       Image
                       {
                           id:lokacijaprofilimage
                          source: slika
                           width: parent.width
                           height: parent.height
                           fillMode: Image.PreserveAspectCrop
                           layer.enabled: true
                           layer.effect: OpacityMask
                           {
                               maskSource: mask
                           }
                       }
                       Rectangle {
                           id: mask
                           width: 50
                           height: 50
                           radius: 30
                           visible: false
                       }
                   }

                   Rectangle
                   {
                       id:nazivrect
                       height: parent.height
                       width: parent.width-parent.height
                       anchors.left: slikarect.right
                       color: "transparent"
                       Text
                       {
                           id:nazivlokacijeprofil
                           text: qsTr(tekst)
                           anchors.left: parent.left
                           anchors.leftMargin: 20
                           anchors.verticalCenter: parent.verticalCenter
                           width: parent.width
                           wrapMode: Text.WordWrap
                       }
                   }

                   MouseArea
                   {

                       width: parent.width-10
                       height: parent.height
                       anchors.left: parent.left
                       onClicked:
                       {
                           block.visible=true
                           funkcija(id)
                           block.visible=false
                           _obavestenjamodel.pregledaj(index)
                           glavnirect.color="white"
                           if(lid==true)
                           {
                                pageLoader.source= "lokacija.qml"
                           }
                           else
                           {
                               pageLoader.source= "dogadjaj.qml"
                           }

                       }

                   }


            }
              /* Rectangle
               {
                   width: parent.width
                   height: 1
                   color: "#c9c9c9"
                   anchors.bottom: parent.bottom
               }
               */
        }
       }
    }
}
}