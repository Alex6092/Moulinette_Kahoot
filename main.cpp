#include <QtCore/QCoreApplication>
#include <qdir.h>
#include <qdebug.h>
#include <qfile.h>
#include <qbytearray.h>
#include <qmap.h>

struct Student
{
    QString name;
    int totalScore; // Le total de point du participant
    int maxScore;   // Le total du maximum atteignable par le participant
    int participation;

    Student(QString name)
    {
        this->name = name;
        totalScore = 0;
        maxScore = 0;
        participation = 0;
    }

    Student()
    {
        name = "";
        totalScore = 0;
        maxScore = 0;
        participation = 0;
    }

    void addScore(int score, int max)
    {
        totalScore += score;
        maxScore += max;
        participation++;
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Récupère la liste des fichiers CSV présents dans le sous-dossier "fichiers" :
    QDir directory("./fichiers");
    QStringList files = directory.entryList(QStringList() << "*.csv" << "*.CSV", QDir::Files);

    QMap<QString, Student> studentsMap;

    QFile studentList("./fichiers/participants.txt");
    if (studentList.open(QIODevice::ReadOnly))
    {
        qInfo("Lecture de la liste des participants ...");

        while (true)
        {
            QByteArray name = studentList.readLine();
            
            // Arrête si on a atteint la fin du fichier :
            if (name.size() == 0)
                break;

            QString nameStr = QString::fromUtf8(name);
            nameStr = nameStr.remove(QRegExp("\\r\\n"));
            nameStr = nameStr.toUpper();
            
            studentsMap[nameStr] = Student(nameStr);
        }

        qInfo("%d participants charges", studentsMap.size());
    }
    else
    {
        qFatal("Echec de la lecture de la liste des participants !");
        abort();
    }

    foreach(QString filename, files)
    {
        qInfo() << "Ouverture du fichier " << filename;
        QString filePath = directory.path() + "/" + filename;
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            qInfo("Lecture du fichier %s", qUtf8Printable(filePath));

            // Ignore les 3 premières lignes :
            for (int i = 0; i < 3; i++)
            {
                file.readLine();
            }

            bool firstLine = true;
            int maxScore = 0;
            while (true)
            {
                // Début des données concernant les étudiants :
                QByteArray data = file.readLine();

                // Arrête si on a atteint la fin du fichier :
                if (data.size() == 0)
                    break;

                QString dataStr = QString::fromUtf8(data);
                dataStr = dataStr.remove(QRegExp("\\r\\n"));
                QRegExp separator(";");
                QStringList splited = dataStr.split(separator);
                if (splited.size() > 3)
                {
                    QString name = splited[1];
                    int score = splited[2].toInt();

                    if (firstLine)
                    {
                        maxScore = score;
                        firstLine = false;
                    }

                    name = name.trimmed().toUpper();
                    int nameLen = name.size();
                    if (!studentsMap.contains(name))
                    {
                        studentsMap[name] = Student(name);
                        qWarning("Etudiant ajoute : %s", qUtf8Printable(name));
                    }
                    
                    studentsMap[name].addScore(score, maxScore);
                }
            }
            
            file.close();
        }
        else
        {
            qFatal("Impossible d'ouvrir le fichier %s", qUtf8Printable(filePath));
        }
    }

    qInfo() << "Récapitulatif :";
    for (auto it = studentsMap.begin(); it != studentsMap.end(); it++)
    {
        Student s = (*it);
        qInfo() << s.name << " : " << s.totalScore << " / " << s.maxScore << " ; nb participation : " << s.participation << " ; % : " << (double)(s.totalScore) / (double)(s.maxScore) * 100.0;
    }

    return a.exec();
}
