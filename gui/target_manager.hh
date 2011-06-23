/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef TARGET_MANAGER_HH
#define TARGET_MANAGER_HH

#include <QtGui/QDialog>
#include <QtCore/QModelIndex>

#include "gui/target_data.hh"

namespace Ui
{
    class TargetManager;
}

class QThread;
class QStandardItemModel;
class QSortFilterProxyModel;
class QErrorMessage;
class QStandardItem;

class DataLoaderHipparcos;
class DataLoaderCSV;
class DataLoaderSimbad;

class TargetManager :
    public QDialog
{
    Q_OBJECT
public:
    TargetManager(QStandardItemModel* targets, QWidget* p = NULL);
    ~TargetManager();

private:
    Ui::TargetManager* _ui;

    QStandardItemModel *_targets, *_hipparcosTargets, *_csvTargets, *_simbadTargets;
    QSortFilterProxyModel *_hipparcosTargetsFilterProxy, *_csvTargetsFilterProxy;

    QString _browseForFile(const QString& filter);
    bool _checkBasicAccess(const QString& filepath) const;

    bool _isHipparcosSourceValid, _isHipparcos2SourceValid, _loadHipparcosData;
    bool _enableHipparcosLoad() const;

    QThread* _loaderThread;
    DataLoaderHipparcos* _hipparcosLoader;
    DataLoaderCSV* _csvLoader;
    DataLoaderSimbad* _simbadLoader;

    QStringList _csvTargetsHeader, _hipparcosTargetsHeader, _simbadTargetsHeader;

    QErrorMessage* _errorMessage;

private slots:
    bool _checkCSVFile(const QString& filepath);
    bool _checkHIPFile(const QString& filepath);
    bool _checkHIP2File(const QString& filepath);

    void _browseForCSV();
    void _browseForHIP();
    void _browseForHIP2();
    void _browseForSimbad();

    void _setLoadHipparcosData(bool load);

    void _loadHipparcos();
    void _loadCSV();

    void _addHipparcosTarget(const TargetDataQt&);
    void _addCSVTarget(const TargetDataQt&);
    void _addSimbadTarget(const TargetDataQt&, const QStringList&);
    void _simbadQueryError(const QString&);

    void _hipparcosTargetSelectionChanged();
    void _csvTargetSelectionChanged();
    void _simbadTargetSelectionChanged();
    void _targetSelectionChanged();

    void _addSelectedHipparcosTarget(const QModelIndex& idx = QModelIndex());
    void _addSelectedCSVTarget(const QModelIndex& idx = QModelIndex());
    void _addSelectedSimbadTarget(const QModelIndex& idx = QModelIndex());

    void _removeTarget(const QModelIndex& idx = QModelIndex());


public slots:
    void removeAllTargets();
    void saveTargets();
    void loadTargets();

    void searchSimbad();
    void searchSimbad(const QString&);

signals:
    void csvSourceChanged(const QString&);
    void hipSourceChanged(const QString&);
    void hip2SourceChanged(const QString&);

    void csvSourceIsValid(bool);
    void hipSourceIsValid(bool);

    void loadHipparcosData(const QString&, const QString&);
    void loadCSVData(const QString&);
    void querySimbad(const QString&);
    void querySimbad(const QStringList&);

    void targetAdded(QStandardItem*);
    void targetRemoved(unsigned int); // TODO: we are emitting the index of the UserRole+2 here, this is bad
    void allTargetsRemoved();
    void numberOfTargetsChanged(int);
};

#endif // TARGET_MANAGER_HH
