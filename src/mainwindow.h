#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "geocode_data_manager.h"

class SortFilterProxyModel;
class ImageListModel;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void goClicked();
    void errorOccured(const QString&);
    void getCoordinates(const QString& address);
    void chooseDirectory();
    void selectImageInView(const QString& filePath);

    void showTagForItem(const QModelIndex &index, bool panToCoordinates = true, bool setMarker = false);
    void onImageAdded(const QModelIndex &parent, int first, int last);

    Ui::MainWindow *ui;
    GeocodeDataManager mGeocodeDataManager;
    ImageListModel* mModel;
    SortFilterProxyModel* mProxyModel;
};

#endif // MAINWINDOW_H
