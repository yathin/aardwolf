/**
    This file is part of Aardwolf.

    Aardwolf is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Aardwolf is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Aardwolf.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "project.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QTextStream>

#include "Helpers/mediator.h"

CurrentProject::CurrentProject() { }

CurrentProject& CurrentProject::instance()
{
    static CurrentProject currentProject;
    return currentProject;
}

Project& CurrentProject::project()
{
    return *(this->currentProject_);
}

bool CurrentProject::initFromFile(const QString &fileName)
{
    this->currentProject_ = QSharedPointer< Project >(new Project(fileName));
    return true;
}

void CurrentProject::create()
{
    this->currentProject_->create();
}

void CurrentProject::setCurrentProject(const Project& project)
{
    this->currentProject_ = QSharedPointer< Project >(new Project(project));
    this->currentProject_->create();
}

CurrentProject::~CurrentProject()
{
    if (this->currentProject_.isNull())
        return;

    this->currentProject_->close();
    this->currentProject_.clear();
}

void CurrentProject::close()
{
    if (this->currentProject_.isNull())
        return;

    this->currentProject_->close();
    this->currentProject_.clear();
}

bool Project::createTableFromFile(const QString &fileName)
{
    if (!QSqlDatabase::database().isOpen()) return false;

    QFile   create(fileName);
    create.open(QIODevice::ReadOnly);
    QString createSQL = QTextStream(&create).readAll();
    create.close();

    QSqlQuery query;
    return query.exec(createSQL);
}

bool Project::insertDB(bool update)
{
    bool ret = false;
    if (update)
    {
        QSqlQuery query;
        query.prepare("UPDATE project SET location = :location, info = :info, xmp = :xmp");
        query.bindValue(":location", this->location);
        query.bindValue(":info", this->info);
        query.bindValue(":xmp", this->xmp);
        ret = query.exec();

        QSqlQuery partitionQuery;
        partitionQuery.prepare("UPDATE partition SET location = :location WHERE name = 'Primary'");
        partitionQuery.bindValue(":location", this->location);
        ret = partitionQuery.exec();
    }
    else
    {
        QSqlQuery query;
        query.prepare("INSERT INTO project(name, location, info, xmp) VALUES (:name, :location, :info, :xmp)");
        query.bindValue(":name", this->name);
        query.bindValue(":location", this->location);
        query.bindValue(":info", this->info);
        query.bindValue(":xmp", this->xmp);
        ret = query.exec();

        QSqlQuery partitionQuery;
        partitionQuery.prepare("INSERT INTO partition(name, location, active) VALUES ('Primary', :location, 1)");
        partitionQuery.bindValue(":location", this->location);
        ret = partitionQuery.exec();

        // NOTE: May not work with all kinds of databases. Works for MySQL/SQLite
        this->id_ = query.lastInsertId().toInt();
    }

    return ret;
}

void Project::create()
{
    if (!QSqlDatabase::contains())
        QSqlDatabase::addDatabase("QSQLITE");

    QSqlDatabase::database().close();
    QSqlDatabase::database().setDatabaseName(this->location + "/" + this->name + ".aardwolf");
    QSqlDatabase::database().open();

    // Check if already has tables
    if (!QSqlDatabase::database().tables().empty())
    {
        // does not need creation
        QMessageBox msgBox;
        msgBox.setText("Error. Database already exists.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    // Create tables and views
    this->createTableFromFile(":/aardwolf/SQL/create_camera.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_folder.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_partition.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_photo.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_project.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_reports.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_reports_default.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_tag.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_tag_definition.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_user.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_xmp.sql");

    this->createTableFromFile(":/aardwolf/SQL/create_data_basic_view.sql");
    this->createTableFromFile(":/aardwolf/SQL/create_folder_tagged_view.sql");

    // Insert project into DB
    this->insertDB();

    // id should be set now
    if (!this->id_)
    {
        // TODO: Show user an error?
        return;
    }

}

Project::Project() { }


Project::Project(const Project& p)
    :
      name(p.name),
      location(p.location),
      xmp(p.xmp),
      id_(p.id_)
{

}

void Project::close()
{
    if (!QSqlDatabase::contains())
        return;

    if (QSqlDatabase::database().isOpen())
        QSqlDatabase::database().close();

    QSqlDatabase::removeDatabase("defaultConnection");
}

Project::Project(const QString &fileName)
{

    if (!QSqlDatabase::contains())
    {
        QSqlDatabase::addDatabase("QSQLITE");
        QSqlDatabase::database().setDatabaseName(fileName);
        QSqlDatabase::database().open();
    }
    else
    {
         if (QSqlDatabase::database().isOpen())
            QSqlDatabase::database().close();

        QSqlDatabase::database().setDatabaseName(fileName);
        QSqlDatabase::database().open();
    }

    QSqlQuery projectQuery;
    projectQuery.prepare("SELECT * FROM project");
    projectQuery.exec();
    if (projectQuery.next())
    {
        this->id_ = projectQuery.value(0).toInt();
        this->name = projectQuery.value(1).toString();
        this->location = projectQuery.value(2).toString();
        this->info = projectQuery.value(3).toString();
        this->xmp = projectQuery.value(4).toBool();
    }

    qxtLog->debug() << " Project ID: " << this->id_
                    << " Name: " << this->name
                    << " XMP: " << this->xmp;


    projectQuery.finish();

    this->tagDef_ = TagDefinition(true);
}

const TagDefinition& Project::getTagDefinition() const
{
    return this->tagDef_;
}

void Project::refreshTagDefinition()
{
    this->tagDef_ = TagDefinition(true);
}

Project::~Project()
{
    this->close();
}

unsigned Project::id()
{
    return id_;
}

void Project::getInfoInHTML(QString &html)
{
    html = "<b>Name:</b> "          + this->name
            + "<br/><b>Location:</b> "   + this->location
            + "<br/><b>Info: </b> "      + this->info;
}

void Project::addToXMPMap(XMPMap &xmpMap)
{
    xmpMap[XMP_PROJECT_NAME]        = this->name;
    xmpMap[XMP_PROJECT_INFO]        = this->info;
}
