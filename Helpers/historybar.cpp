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

#include "historyBar.h"

#include <QLayout>
#include <QPixmap>

#include <QSize>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>

#include "Helpers/mediator.h"

HistoryBar   *HistoryBar::historyBar = 0;

HistoryBar::HistoryBar()
    : QObject(),
      container_(0),
      projectIcon_(0),
      cameraIcon_(0),
      folderIcon_(0),
      hLayout_(0)
{

}

HistoryBar::~HistoryBar()
{
    delete this->projectIcon_;
    delete this->cameraIcon_;
    delete this->folderIcon_;
}

HistoryBar& HistoryBar::instance()
{
    return *historyBar;
}

void    HistoryBar::init(QWidget *container)
{
    if (historyBar) delete historyBar;

    historyBar = new HistoryBar();

    historyBar->container_ = container;

    historyBar->projectIcon_ = new QPushButton();
    historyBar->projectIcon_->setVisible(false);
    historyBar->projectIcon_->setIcon(QIcon(QPixmap(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-actions-go-home.svg.png")));
    historyBar->projectIcon_->setFlat(true);
    historyBar->projectIcon_->setStyleSheet("padding-left: 0px");
    historyBar->projectIcon_->setIconSize(QSize(32,32));
    historyBar->projectIcon_->setMinimumHeight(32);
    connect(historyBar->projectIcon_, SIGNAL(clicked()), historyBar, SLOT(projectClicked()));


    historyBar->cameraIcon_ = new QPushButton(container);
    historyBar->cameraIcon_->setVisible(false);
    historyBar->cameraIcon_->setIcon(QIcon(QPixmap(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-devices-camera-photo.svg.png")));
    historyBar->cameraIcon_->setFlat(true);
    historyBar->cameraIcon_->setStyleSheet("padding-left: 0px");
    historyBar->cameraIcon_->setIconSize(QSize(32,32));
    historyBar->cameraIcon_->setMinimumHeight(32);
    connect(historyBar->cameraIcon_, SIGNAL(clicked()), historyBar, SLOT(cameraClicked()));

    historyBar->folderIcon_ = new QPushButton(container);
    historyBar->folderIcon_->setVisible(false);
    historyBar->folderIcon_->setIcon(QIcon(QPixmap(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-places-folder-image.svg.png")));
    historyBar->folderIcon_->setFlat(true);
    // historyBar->folderIcon_->setEnabled(false);
    historyBar->folderIcon_->setStyleSheet("padding-left: 0px");
    historyBar->folderIcon_->setIconSize(QSize(32,32));
    historyBar->folderIcon_->setMinimumHeight(32);
    connect(historyBar->folderIcon_, SIGNAL(clicked()), historyBar, SLOT(folderClicked()));

    historyBar->searchIcon_ = new QPushButton(container);
    historyBar->searchIcon_->setVisible(false);
    historyBar->searchIcon_->setIcon(QIcon(QPixmap(":/Oxygen/Files/icons/oxygen/120px-Oxygen480-actions-document-preview.svg.png")));
    historyBar->searchIcon_->setFlat(true);
    // historyBar->folderIcon_->setEnabled(false);
    historyBar->searchIcon_->setStyleSheet("padding-left: 0px");
    historyBar->searchIcon_->setIconSize(QSize(32,32));
    historyBar->searchIcon_->setMinimumHeight(32);
    connect(historyBar->searchIcon_, SIGNAL(clicked()), historyBar, SLOT(searchClicked()));

}


void HistoryBar::close()
{
    delete historyBar;
    historyBar = 0;
}


void HistoryBar::refreshLayout()
{
    if (hLayout_ != 0)
    {
        delete hLayout_;
    }

    hLayout_ = new QHBoxLayout(this->container_);

    hLayout_->addWidget(this->projectIcon_);
    hLayout_->addWidget(this->cameraIcon_);
    hLayout_->addWidget(this->folderIcon_);
    hLayout_->addWidget(this->searchIcon_);

    hLayout_->addStretch();
}

void HistoryBar::setLabel(QPushButton *label, const QString& text)
{
    if (text.isEmpty())
        label->setVisible(false);
    else
    {
        label->setText(text + " > ");
        label->setVisible(true);
    }
}

void HistoryBar::setProject(const QString &project)
{
    this->setCamera("");
    this->setFolder("");
    this->setLabel(this->projectIcon_, project);
    this->refreshLayout();
    Mediator::instance().enableFileFilter(false);
    Mediator::instance().enableDirFilter(false);
}

void HistoryBar::setCamera(const QString &camera)
{
    this->setFolder("");
    this->setLabel(this->cameraIcon_, camera);
    this->refreshLayout();
    Mediator::instance().enableFileFilter(false);
    Mediator::instance().enableDirFilter(true);
}

void HistoryBar::setFolder(const QString &folder)
{
    this->setLabel(this->folderIcon_, folder);
    this->setSearch("");
    this->refreshLayout();
    if (folder.isEmpty())
    {
        Mediator::instance().enableFileFilter(false);
        Mediator::instance().enableDirFilter(true);
    }
    else
    {
        Mediator::instance().enableFileFilter(true);
        Mediator::instance().enableDirFilter(false);
    }
}

void HistoryBar::setSearch(const QString& search)
{
    this->setLabel(this->searchIcon_, search);
    this->refreshLayout();
}

void HistoryBar::projectClicked()
{
    setFolder("");
    setCamera("");
    Mediator::instance().refreshProject();
}

void HistoryBar::cameraClicked()
{
    setFolder("");
    Mediator::instance().refreshCamera();
}

void HistoryBar::folderClicked()
{
    Mediator::instance().refreshFolder();
}

void HistoryBar::searchClicked()
{

}
