#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagelistmodel.h"
#include "sortfilterproxymodel.h"
#include "exifwrapper.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::chooseDirectory);
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::chooseDirectory);
    connect(ui->actionAbout, &QAction::triggered, [&](){
        QString msg = "<html><head/><body><p><span style=\" font-weight:600;\">PhotoTag</span> ver. 1.0</p><p>This is free software; you can redistribute it and/or modify it under the the terms of the <a href=\"http://dev.perl.org/licenses/gpl1.html\">"
        "<span style=\" text-decoration: underline; color:#0000ff;\">GNU General Public License</span></a></p><p>Please send any feedback or bug reports to <a href=\"phototag@outlook.de\"><span style=\" text-decoration: underline; color:#0000ff;\">phototag@outlook.de</span></a></p></body></html>";
        QMessageBox::information(this, "PhotoTag", msg);
    });

    connect(ui->goButton, &QPushButton::clicked, this, &MainWindow::goClicked);
    connect(ui->postalAddress, &QLineEdit::returnPressed, this, &MainWindow::goClicked);
    connect(ui->postalAddress, &QLineEdit::textChanged, [&](const QString& text){ ui->goButton->setEnabled(!text.isEmpty());});

    connect(&mGeocodeDataManager, &GeocodeDataManager::coordinatesReady, ui->webView, &DropableWebView::goToCoordinates);
    connect(&mGeocodeDataManager, &GeocodeDataManager::errorOccured, this, &MainWindow::errorOccured);
    connect(&mGeocodeDataManager, &GeocodeDataManager::addressReady, ui->postalAddress, &QLineEdit::setText);

    QFile html(":/html/google_maps.html");
    if(html.open(QIODevice::ReadOnly))
    {
        QString str = html.readAll();
        ui->webView->setHtml(str, QUrl("qrc:/"));
    }

    mProxyModel = new SortFilterProxyModel(this);
    mModel = new ImageListModel();
    mProxyModel->setSourceModel(mModel);
    ui->listView->setModel(mProxyModel);
    mProxyModel->sort(0);

    ui->listView->setGridSize(mModel->data(QModelIndex(), Qt::SizeHintRole).toSize());

    connect(ui->webView, &DropableWebView::fileChanged, mModel, &ImageListModel::onFileChanged);
    connect(ui->webView, &DropableWebView::imageClickedOnMap, this, &MainWindow::selectImageInView);
    connect(ui->hideTaggedPhotos, &QCheckBox::toggled, mProxyModel, &SortFilterProxyModel::setFilterActive);
    connect(ui->listView, &QAbstractItemView::clicked, [this] (const QModelIndex& index) { showTagForItem(index); });
    connect(mModel, &QAbstractItemModel::modelReset, ui->webView, &DropableWebView::removeAllMarkers);
    connect(mModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onImageAdded);
    connect(mModel, &QAbstractItemModel::dataChanged, [this] (const QModelIndex &topLeft, const QModelIndex &bottomRight)
    {
        onImageAdded(QModelIndex(), topLeft.row(), bottomRight.row());
    });

    QTimer::singleShot(0, [&]() {
        if(!ExifWrapper::isExifAvailable())
        {
            QString msg = "<html><head/><body><p>The EXIF application required for this tool is not found.</p>"
                          "<p>Please visit <a href=\"https://www.sno.phy.queensu.ca/~phil/exiftool/\"><span style=\" text-decoration: underline; color:#0000ff;\">https://www.sno.phy.queensu.ca/~phil/exiftool</span></a> and follow the instructions to download and install the executable in the same directory as this application.</p></body></html>";
            QMessageBox::critical(this, "Warning",msg);
            qApp->quit();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mModel;
}

void MainWindow::goClicked()
{
    QString address = ui->postalAddress->text();
    mGeocodeDataManager.getCoordinates(address.replace(" ", "+"));
}

void MainWindow::errorOccured(const QString& error)
{
    QMessageBox::warning(this, tr("Geocode Error"), error);
}

void MainWindow::chooseDirectory()
{
    auto dir = QFileDialog::getExistingDirectory(this);
    if(!dir.isEmpty())
        mModel->setDirectory(dir);
}

void MainWindow::selectImageInView(const QString &filePath)
{
    if(auto model = ui->listView->model())
    {
        for(int row = 0; row<model->rowCount(); row++)
        {
            auto idx = model->index(row, 0);
            auto fName = model->data(idx, Qt::DisplayRole).toString();
            if(filePath == fName)
            {
                ui->listView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
                ui->listView->scrollTo(idx);
                break;
            }
        }
    }
}

void MainWindow::showTagForItem(const QModelIndex &index, bool panToCoordinates, bool setMarker)
{
    QModelIndex idx = index;
    if(index.model() == mProxyModel)
        idx = mProxyModel->mapToSource(index);
    if(mModel->data(idx, ImageListModel::HasGeoTag).toBool())
    {
        auto lat = mModel->data(idx, ImageListModel::LatitudeRole).toDouble();
        auto lng = mModel->data(idx, ImageListModel::LongitudeRole).toDouble();
        auto fileName = mModel->data(idx, Qt::DisplayRole).toString();
        if(panToCoordinates)
            ui->webView->goToCoordinates(lat,lng);
        if(setMarker)
            ui->webView->setMarker(lat, lng, fileName);
    }
}

void MainWindow::onImageAdded(const QModelIndex &parent, int first, int last)
{
    for(int row=first ; row<=last; row++)
    {
        showTagForItem(mModel->index(row), false, true);
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    ui->webView->waitForFinished();
    ev->accept();
}
