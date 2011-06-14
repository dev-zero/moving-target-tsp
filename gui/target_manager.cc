/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QtGui/QFileDialog>
#include <QtCore/QtConcurrentRun>
#include <QtGui/QStandardItemModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QErrorMessage>
#include <QtGui/QMessageBox>
#include <QtXml/QDomDocument>

#include "3rdparty/qtsoap/qtsoap.h"

#include "gui/target_manager.hh"
#include "sources/file_source.hh"
#include "gui/data_loader_hipparcos.hh"
#include "gui/data_loader_csv.hh"
#include "utils/common_calculation_functions.hh"

#include "ui_target_manager.h"

TargetManager::TargetManager(QStandardItemModel* targets, QWidget* p) :
    QDialog(p),
    _targets(targets),
    _isHipparcosSourceValid(false),
    _isHipparcos2SourceValid(false)
{
    _ui = new Ui::TargetManager;
    _ui->setupUi(this);

    _errorMessage = new QErrorMessage(this);

    _hipparcosLoader = new DataLoaderHipparcos;
    _csvLoader = new DataLoaderCSV;

    _loaderThread = new QThread;
    _hipparcosLoader->moveToThread(_loaderThread);
    _csvLoader->moveToThread(_loaderThread);

    _ui->targets->setModel(_targets);

    _hipparcosTargetsHeader << "Hipparcos ID" << "Position" << "Velocity";
    _csvTargetsHeader << "Name" << "Position" << "Velocity";
    _simbadTargetsHeader << "Name" << "Right Ascension" << "Declination" << "Radial Velocity";
    _simbadTargetsHeader << "PM Plx" << "PM RA" << "PM DE" << "Position" << "Velocity";

    _hipparcosTargets = new QStandardItemModel(0, _hipparcosTargetsHeader.count(), this);
    _hipparcosTargets->setHorizontalHeaderLabels(_hipparcosTargetsHeader);
    _hipparcosTargetsFilterProxy = new QSortFilterProxyModel(this);
    _hipparcosTargetsFilterProxy->setSourceModel(_hipparcosTargets);
    _ui->hipparcosTargets->setModel(_hipparcosTargetsFilterProxy);

    _csvTargets = new QStandardItemModel(0, _csvTargetsHeader.count(), this);
    _csvTargets->setHorizontalHeaderLabels(_csvTargetsHeader);
    _csvTargetsFilterProxy = new QSortFilterProxyModel(this);
    _csvTargetsFilterProxy->setDynamicSortFilter(true);
    _csvTargetsFilterProxy->setSourceModel(_csvTargets);
    _ui->csvTargets->setModel(_csvTargetsFilterProxy);

    _simbadTargets = new QStandardItemModel(0, _simbadTargetsHeader.count(), this);
    _simbadTargets->setHorizontalHeaderLabels(_simbadTargetsHeader);
    _ui->simbadTargets->setModel(_simbadTargets);

    _http = new QtSoapHttpTransport(this);
    _http->setAction("urn:simbad");
    _http->setHost("cdsws.u-strasbg.fr");

    connect(_ui->browseForCSV, SIGNAL(clicked()), SLOT(_browseForCSV()));
    connect(_ui->browseForHipparcos, SIGNAL(clicked()), SLOT(_browseForHIP()));
    connect(_ui->browseForHipparcos2, SIGNAL(clicked()), SLOT(_browseForHIP2()));

    connect(this, SIGNAL(csvSourceChanged(const QString&)), _ui->csvFilepath, SLOT(setText(const QString&)));
    connect(this, SIGNAL(hipSourceChanged(const QString&)), _ui->hipparcosFilepath, SLOT(setText(const QString&)));
    connect(this, SIGNAL(hip2SourceChanged(const QString&)), _ui->hipparcos2Filepath, SLOT(setText(const QString&)));

    connect(_ui->csvFilepath, SIGNAL(textChanged(const QString&)), SLOT(_checkCSVFile(const QString&)));
    connect(_ui->hipparcosFilepath, SIGNAL(textChanged(const QString&)), SLOT(_checkHIPFile(const QString&)));
    connect(_ui->hipparcos2Filepath, SIGNAL(textChanged(const QString&)), SLOT(_checkHIP2File(const QString&)));

    connect(_ui->hipparcosGroup, SIGNAL(toggled(bool)), SLOT(_setLoadHipparcosData(bool)));

    connect(this, SIGNAL(csvSourceIsValid(bool)), _ui->loadCSV, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(hipSourceIsValid(bool)), _ui->loadHipparcos, SLOT(setEnabled(bool)));

//    connect(_ui->loadCSV, SIGNAL(clicked()), SLOT(disableCSVLoadInterface()));
//    connect(_ui->loadHIP, SIGNAL(clicked()), SLOT(disableHIPLoadInterface()));

    connect(_ui->loadHipparcos, SIGNAL(clicked()), SLOT(_loadHipparcos()));
    connect(_ui->loadCSV, SIGNAL(clicked()), SLOT(_loadCSV()));

    connect(_ui->hipparcosTargetFilter, SIGNAL(textChanged(const QString&)), _hipparcosTargetsFilterProxy, SLOT(setFilterRegExp(const QString&)));
    connect(_ui->csvTargetFilter, SIGNAL(textChanged(const QString&)), _csvTargetsFilterProxy, SLOT(setFilterRegExp(const QString&)));

    connect(this, SIGNAL(loadHipparcosData(const QString&, const QString&)), _hipparcosLoader, SLOT(load(const QString&, const QString&)));
    connect(this, SIGNAL(loadCSVData(const QString&)), _csvLoader, SLOT(load(const QString&)));

    connect(_hipparcosLoader, SIGNAL(targetFound(const TargetDataQt&)), SLOT(_addHipparcosTarget(const TargetDataQt&)));
    connect(_csvLoader, SIGNAL(targetFound(const TargetDataQt&)), SLOT(_addCSVTarget(const TargetDataQt&)));

    connect(_ui->csvTargets->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            SLOT(_csvTargetSelectionChanged()));
    connect(_ui->hipparcosTargets->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            SLOT(_hipparcosTargetSelectionChanged()));
    connect(_ui->simbadTargets->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            SLOT(_simbadTargetSelectionChanged()));
    connect(_ui->targets->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            SLOT(_targetSelectionChanged()));

    connect(_ui->csvTargets, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(_addSelectedCSVTarget(const QModelIndex&)));
    connect(_ui->hipparcosTargets, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(_addSelectedHipparcosTarget(const QModelIndex&)));
    connect(_ui->simbadTargets, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(_addSelectedSimbadTarget(const QModelIndex&)));
    connect(_ui->addCSVTarget, SIGNAL(clicked()), SLOT(_addSelectedCSVTarget()));
    connect(_ui->addHipparcosTarget, SIGNAL(clicked()), SLOT(_addSelectedHipparcosTarget()));
    connect(_ui->addSimbadTarget, SIGNAL(clicked()), SLOT(_addSelectedSimbadTarget()));

    connect(_ui->removeTarget, SIGNAL(clicked()), SLOT(_removeTarget()));
    connect(_ui->removeAllTargets, SIGNAL(clicked()), SLOT(removeAllTargets()));
    connect(_ui->targets, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(_removeTarget(const QModelIndex&)));

    connect(_ui->loadTargets, SIGNAL(clicked()), SLOT(loadTargets()));
    connect(_ui->saveTargets, SIGNAL(clicked()), SLOT(saveTargets()));

    connect(_ui->querySimbad, SIGNAL(clicked()), SLOT(searchSimbad()));
    connect(_ui->simbadQuery, SIGNAL(returnPressed()), SLOT(searchSimbad()));

    connect(_http, SIGNAL(responseReady()), SLOT(_getSimbadResponse()));

    _loaderThread->start();
}

TargetManager::~TargetManager()
{
    _loaderThread->quit();
    _loaderThread->wait();
    delete _loaderThread;
    delete _hipparcosLoader;
    delete _csvLoader;
}

bool TargetManager::_checkBasicAccess(const QString& filepath) const
{
    QFile f(filepath);
    return (f.exists() && f.open(QIODevice::ReadOnly));
}

bool TargetManager::_checkCSVFile(const QString& filepath)
{
    bool access(_checkBasicAccess(filepath));
    emit csvSourceIsValid(access);
    return access;
}
bool TargetManager::_enableHipparcosLoad() const
{
    return (_isHipparcos2SourceValid && (_isHipparcosSourceValid || !_loadHipparcosData));
}
bool TargetManager::_checkHIPFile(const QString& filepath)
{
    _isHipparcosSourceValid = _checkBasicAccess(filepath); 
    emit hipSourceIsValid(_enableHipparcosLoad());
    return _isHipparcosSourceValid;
}
bool TargetManager::_checkHIP2File(const QString& filepath)
{
    _isHipparcos2SourceValid = _checkBasicAccess(filepath); 
    emit hipSourceIsValid(_enableHipparcosLoad());
    return _isHipparcos2SourceValid;
}
void TargetManager::_setLoadHipparcosData(bool load)
{
    _loadHipparcosData = load;
    emit hipSourceIsValid(_enableHipparcosLoad());
}

QString TargetManager::_browseForFile(const QString& filter)
{
    return QFileDialog::getOpenFileName(this,
            tr("Open data file"),
            QString(),
            filter);
}
void TargetManager::_browseForCSV()
{
    QString filepath(_browseForFile(tr("Comma-separated value list (*.txt *.csv)")));
    if (filepath.size() > 0)
        emit csvSourceChanged(filepath);
}
void TargetManager::_browseForHIP()
{
    QString filepath(_browseForFile(tr("Hipparcos data (*.dat)")));
    if (filepath.size() > 0)
        emit hipSourceChanged(filepath);
}
void TargetManager::_browseForHIP2()
{
    QString filepath(_browseForFile(tr("Hipparcos-2 data (*.dat)")));
    if (filepath.size() > 0)
        emit hip2SourceChanged(filepath);
}

void TargetManager::_loadHipparcos()
{
    _hipparcosTargets->clear();
    _hipparcosTargets->setHorizontalHeaderLabels(_hipparcosTargetsHeader);
    emit loadHipparcosData(_ui->hipparcos2Filepath->text(), _ui->hipparcosFilepath->text());
}

void TargetManager::_loadCSV()
{
    _csvTargets->clear();
    _csvTargets->setHorizontalHeaderLabels(_csvTargetsHeader);
    emit loadCSVData(_ui->csvFilepath->text());
}

void TargetManager::_addCSVTarget(const TargetDataQt& t)
{
    qDebug() << "TargetManager::_addCSVTarget";
    QString line(t.name);

    /* if the additional data contains a description,
     * append it to the name */

    QMap<QString, QVariant>::const_iterator f;

    f = t.data.find("description");
    if (f != t.data.end())
        line += QString(" (%1)").arg(f->toString());

    QStandardItem* item(new QStandardItem(line));
    item->setData(QVariant::fromValue(t), Qt::UserRole + 1);
    QList<QStandardItem *> items;
    items << item;
    items << new QStandardItem(QString("%1/%2/%3").arg(t.position[0]).arg(t.position[1]).arg(t.position[2]));
    items << new QStandardItem(QString("%1/%2/%3").arg(t.velocity[0]).arg(t.velocity[1]).arg(t.velocity[2]));
    /*
    unsigned int idx(_ui->renderingWidget->addTarget(t));
    item->setData(QVariant::fromValue(idx), Qt::UserRole + 2); // first role idx is used for TargetDataQt
*/
    _csvTargets->appendRow(items);
}

void TargetManager::_addHipparcosTarget(const TargetDataQt& t)
{
    /* if the additional data contains a description,
     * append it to the name */
/*
    QMap<QString, QVariant>::const_iterator f;

    f = t.data.find("description");
    if (f != t.data.end())
        line += QString(" (%1)").arg(f->toString());
*/
    QStandardItem* item(new QStandardItem(t.name));
    item->setData(QVariant::fromValue(t), Qt::UserRole + 1);
    QList<QStandardItem *> items;
    items << item;
    items << new QStandardItem(QString("%1/%2/%3").arg(t.position[0]).arg(t.position[1]).arg(t.position[2]));
    items << new QStandardItem(QString("%1/%2/%3").arg(t.velocity[0]).arg(t.velocity[1]).arg(t.velocity[2]));
/*
    unsigned int idx(_ui->renderingWidget->addTarget(t));
    item->setData(QVariant::fromValue(idx), Qt::UserRole + 2); // first role idx is used for TargetDataQt
*/
    _hipparcosTargets->appendRow(items);
}

void TargetManager::removeAllTargets()
{
    _targets->removeRows(0, _targets->rowCount());
    emit allTargetsRemoved();
    emit numberOfTargetsChanged(0);
}

void TargetManager::_removeTarget(const QModelIndex& idx)
{
    int row(-1);

    if (idx.isValid())
        row = idx.row();
    else
        row = _ui->targets->selectionModel()->selectedRows().front().row();

    if (row < 0)
    {
        qDebug() << "TargetManager::_removeTarget could not determine a valid row number, this should not happen!";
        return;
    }

    emit targetRemoved(_targets->data(_targets->index(row, 0), Qt::UserRole + 2).value<unsigned int>());

    _targets->removeRow(row);

    emit numberOfTargetsChanged(_targets->rowCount());
}


void TargetManager::_csvTargetSelectionChanged()
{
    _ui->addCSVTarget->setEnabled(_ui->csvTargets->selectionModel()->hasSelection());
}

void TargetManager::_hipparcosTargetSelectionChanged()
{
    _ui->addHipparcosTarget->setEnabled(_ui->hipparcosTargets->selectionModel()->hasSelection());
}

void TargetManager::_simbadTargetSelectionChanged()
{
    _ui->addSimbadTarget->setEnabled(_ui->simbadTargets->selectionModel()->hasSelection());
}

void TargetManager::_targetSelectionChanged()
{
    _ui->removeTarget->setEnabled(_ui->targets->selectionModel()->hasSelection());
}

void TargetManager::_addSelectedHipparcosTarget(const QModelIndex& idx)
{
    QList<QStandardItem *> itemList;
    if (idx.isValid())
        itemList = _hipparcosTargets->takeRow(idx.row());
    else
        itemList = _hipparcosTargets->takeRow(_ui->hipparcosTargets->selectionModel()->selectedRows().front().row());

    _targets->appendRow(itemList);
    emit targetAdded(itemList.front());
    emit numberOfTargetsChanged(_targets->rowCount());
}

void TargetManager::_addSelectedCSVTarget(const QModelIndex& idx)
{
    QList<QStandardItem *> itemList;
    if (idx.isValid())
        itemList = _csvTargets->takeRow(idx.row());
    else
        itemList = _csvTargets->takeRow(_ui->csvTargets->selectionModel()->selectedRows().front().row());

    _targets->appendRow(itemList);
    emit targetAdded(itemList.front());
    emit numberOfTargetsChanged(_targets->rowCount());
}

void TargetManager::_addSelectedSimbadTarget(const QModelIndex& idx)
{
    int row(-1);
    if (idx.isValid())
        row = idx.row();
    else
        row = _ui->simbadTargets->selectionModel()->selectedRows().front().row();

    if (!_simbadTargets->data(_simbadTargets->index(row, 0), Qt::UserRole + 1).isValid())
    {
        QMessageBox::warning(this, tr("Incomplete object data"), tr("The selection object has incomplete data and can not be used for calculations."));
        return;
    }

    // TODO: we are possibly leaking memory here
    QList<QStandardItem*> originalItemList(_simbadTargets->takeRow(row));
    QList<QStandardItem*> itemList;
    itemList << originalItemList.at(0) << originalItemList.at(_simbadTargetsHeader.count()-2) << originalItemList.at(_simbadTargetsHeader.count()-1);
    _targets->appendRow(itemList);
    emit targetAdded(itemList.front());
    emit numberOfTargetsChanged(_targets->rowCount());
}

void TargetManager::saveTargets()
{
    qDebug() << "saveTargets";

    QFile file(QFileDialog::getSaveFileName(this,
                tr("Select file for saving the target list"),
                QString(),
                tr("Moving-Target TSP Targets File (*.mttsp)")));

    if (file.fileName().size() == 0)
        return;

    if (!file.open(QIODevice::WriteOnly))
    {
        _errorMessage->showMessage(tr("The selected file could not be opened for writing."));
        return;
    }

    QDataStream out(&file);

    out << static_cast<quint64>(0x4D54545350); // my magic bytes: "MTTSP" as ASCII
    out << static_cast<qint32>(1); // the file format version

    const int rowCount(_targets->rowCount());
    out << rowCount;

    for (int i(0); i < rowCount; ++i)
    {
        QModelIndex idx(_targets->index(i, 0));
        TargetDataQt t(_targets->data(idx, Qt::UserRole + 1).value<TargetDataQt>());
        out << t.name;
        out << t.position[0] << t.position[1] << t.position[2];
        out << t.velocity[0] << t.velocity[1] << t.velocity[2];
        out << t.data;
    }
}

void TargetManager::loadTargets()
{
    qDebug() << "loadTargets";

    removeAllTargets();

    QFile file(QFileDialog::getOpenFileName(this,
                tr("Select file for loading the target list"),
                QString(),
                tr("Moving-Target TSP Targets File (*.mttsp)")));

    if (file.fileName().size() == 0)
        return;

    if (!file.open(QIODevice::ReadOnly))
    {
        _errorMessage->showMessage(tr("File could not be opened for reading."));
        return;
    }
    QDataStream in(&file);

    quint64 magic;
    in >> magic;
    if (magic != 0x4D54545350)
    {
        _errorMessage->showMessage(tr("Invalid file selected."));
        return;
    }

    qint32 version;
    in >> version;
    if (version != 1)
    {
        _errorMessage->showMessage(tr("Invalid file version."));
        return;
    }

    int rowCount;
    in >> rowCount;

    for (int i(0); i < rowCount; ++i)
    {
        QString name;
        std::array<double,3> position, velocity;
        QMap<QString, QVariant> t_data;

        in >> name;
        in >> position[0] >> position[1] >> position[2];
        in >> velocity[0] >> velocity[1] >> velocity[2];
        in >> t_data;

        qDebug() << "found target with name " << name << " in the stream";

        TargetDataQt t(position, velocity, name);
        t.data = t_data;

        QStandardItem* item(new QStandardItem(t.name));
        item->setData(QVariant::fromValue(t), Qt::UserRole + 1);
        QList<QStandardItem *> items;
        items << item;
        items << new QStandardItem(QString("%1/%2/%3").arg(t.position[0]).arg(t.position[1]).arg(t.position[2]));
        items << new QStandardItem(QString("%1/%2/%3").arg(t.velocity[0]).arg(t.velocity[1]).arg(t.velocity[2]));

        _targets->appendRow(items);
        emit targetAdded(item);
    }
    emit numberOfTargetsChanged(_targets->rowCount());
}

void TargetManager::searchSimbad()
{
    searchSimbad(_ui->simbadQuery->text());
}

void TargetManager::searchSimbad(const QString& query)
{
    if (query == "")
    {
        QMessageBox::warning(this, tr("Missing search string"), tr("Please enter a search string."));
        return;
    }

    QtSoapMessage request;
    request.setMethod(QtSoapQName("sesame", "urn:sesame"));
    request.addMethodArgument("name", "", query);
    request.addMethodArgument("resultType", "", "x");
    _http->submitRequest(request, "/axis/services/Sesame");

    _simbadTargets->clear();
    _simbadTargets->setHorizontalHeaderLabels(_simbadTargetsHeader);
}

void TargetManager::_getSimbadResponse()
{
    const QtSoapMessage& resp(_http->getResponse());
    if (resp.isFault())
    {
        QMessageBox::warning(this, "webservice error", "soap request failed: " + resp.faultString().value().toString());
        return;
    }

    const QtSoapType& res(resp.returnValue());
    if (!res.isValid())
    {
        QMessageBox::warning(this, "webservice error", "soap return type is invalid");
        return;
    }

    qDebug() << res.value().toString();

    QDomDocument doc;
    doc.setContent(res.value().toString());


    QDomNodeList targets(doc.elementsByTagName("Target"));

    for (int i(0); i < targets.count(); ++i)
    {
        QDomNode target_node(targets.at(i));
        QDomNode resolver = target_node.namedItem("Resolver");

        if (resolver.isNull())
        {
            qDebug() << "no object found";
            continue;
        }

        QList<QStandardItem *> items;
        QStandardItem* item(new QStandardItem(resolver.firstChildElement("oname").text()));

        items << item;
        items << new QStandardItem(resolver.firstChildElement("jradeg").text());
        items << new QStandardItem(resolver.firstChildElement("jdedeg").text());

        QDomNode radialVelocity = resolver.namedItem("Vel");
        if (radialVelocity.isNull())
            items << new QStandardItem("(unavailable)");
        else
            items << new QStandardItem(radialVelocity.firstChildElement("v").text());

        QDomNode properMotions = resolver.namedItem("pm");
        if (properMotions.isNull())
        {
            items << new QStandardItem("(unavailable)");
            items << new QStandardItem("(unavailable)");
            items << new QStandardItem("(unavailable)");
            items << new QStandardItem("(unavailable)");
            items << new QStandardItem("(unavailable)");
        }
        else
        {
            items << new QStandardItem(properMotions.firstChildElement("pa").text());
            items << new QStandardItem(properMotions.firstChildElement("pmRA").text());
            items << new QStandardItem(properMotions.firstChildElement("pmDE").text());

            // we need the parallax contained in the proper motions to calculate the cartesian coordinates
            std::array<double,3> position, velocity;
            equatorial2cartesian(
                    resolver.firstChildElement("jdedeg").text().toDouble(),
                    resolver.firstChildElement("jradeg").text().toDouble(),
                    properMotions.firstChildElement("pa").text().toDouble(),
                    0.0, // let the parallax error be 0 since we don't have the exact value
                    position[0], position[1], position[2]);
            equatorial2cartesian(
                    resolver.firstChildElement("jdedeg").text().toDouble() + (properMotions.firstChildElement("pmDE").text().toDouble()/(3600000.0*180.0)*M_PI),
                    resolver.firstChildElement("jradeg").text().toDouble() + (properMotions.firstChildElement("pmRA").text().toDouble()/(3600000.0*180.0)*M_PI),
                    properMotions.firstChildElement("pa").text().toDouble(),
                    0.0, // let the parallax error be 0 since we don't have the exact value
                    velocity[0], velocity[1], velocity[2]);

            velocity[0] -= position[0];
            velocity[1] -= position[1];
            velocity[2] -= position[2];

            items << new QStandardItem(QString("%1/%2/%3").arg(position[0]).arg(position[1]).arg(position[2]));
            items << new QStandardItem(QString("%1/%2/%3").arg(velocity[0]).arg(velocity[1]).arg(velocity[2]));

            TargetDataQt t(position, velocity, resolver.firstChildElement("oname").text());
            item->setData(QVariant::fromValue(t), Qt::UserRole + 1);
        }
        _simbadTargets->appendRow(items);
    }
}

