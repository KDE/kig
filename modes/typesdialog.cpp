/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "typesdialog.h"

#include "edittype.h"
#include "ui_typeswidget.h"
#include "../kig/kig_part.h"
#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"
#include "../kig/kig_document.h"

#include <algorithm>
#include <iterator>

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

#include <KConfigGroup>
#include <KHelpClient>
#include <KIconLoader>
#include <KMessageBox>

#ifdef WITH_GEOGEBRA
#include "../geogebra/geogebratransformer.h"

#include <QDebug>
#include <QXmlQuery>

#include <KZip>
#endif //WITH_GEOGEBRA

static QString wrapAt( const QString& str, int col = 50 )
{
  QStringList ret;
  int delta = 0;
  while ( delta + col < str.length() )
  {
    int pos = delta + col;
    while ( !str.at( pos ).isSpace() ) --pos;
    ret << str.mid( delta, pos - delta );
    delta = pos + 1;
  }
  ret << str.mid( delta );
  return ret.join( QStringLiteral("<br>") );
}

class BaseListElement
{
protected:
  BaseListElement();

public:
  virtual ~BaseListElement();

  virtual bool isMacro() const { return false; }
  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual QString icon( bool canNull = false ) const = 0;
  virtual QString type() const = 0;
};

BaseListElement::BaseListElement()
{
}

BaseListElement::~BaseListElement()
{
}

class MacroListElement
  : public BaseListElement
{
  Macro* mmacro;
public:
  MacroListElement( Macro* m );
  virtual ~MacroListElement();

  Macro* getMacro() const { return mmacro; }
  bool isMacro() const Q_DECL_OVERRIDE { return true; }
  QString name() const Q_DECL_OVERRIDE;
  QString description() const Q_DECL_OVERRIDE;
  QString icon( bool canNull = false ) const Q_DECL_OVERRIDE;
  QString type() const Q_DECL_OVERRIDE;
};

MacroListElement::MacroListElement( Macro* m )
  : BaseListElement(), mmacro( m )
{
}

MacroListElement::~MacroListElement()
{
  // do NOT delete the associated macro, but instead safely release the
  // pointer, as it's kept elsewhere
  mmacro = 0;
}

QString MacroListElement::name() const
{
  return mmacro->action->descriptiveName();
}

QString MacroListElement::description() const
{
  return mmacro->action->description();
}

QString MacroListElement::icon( bool canNull ) const
{
  return mmacro->ctor->iconFileName( canNull );
}

QString MacroListElement::type() const
{
  return i18n( "Macro" );
}


TypesModel::TypesModel( QObject* parent )
  : QAbstractTableModel( parent )
{
}

TypesModel::~TypesModel()
{
}

void TypesModel::addMacros( const std::vector<Macro*>& macros )
{
  if ( macros.size() < 1 )
    return;

  beginInsertRows( QModelIndex(), melems.size(), melems.size() + macros.size() - 1 );

  for ( std::vector<Macro*>::const_iterator it = macros.begin();
        it != macros.end(); ++it )
  {
    melems.push_back( new MacroListElement( *it ) );
  }

  endInsertRows();
}

void TypesModel::removeElements( const QModelIndexList& elems )
{
  // this way of deleting needs some explanation: the std::vector.erase needs
  // an iterator to the element to remove from the list, while the
  // beginRemoveRows() of Qt needs the index(es). so, for each element to
  // delete, we search it into melems (keeping a count of the id), and when we
  // find it, we free the memory of the BaseListElement and remove the element
  // from the list. in the meanwhile, we notify the model structure of Qt that
  // we're removing a row.
  for ( int i = elems.count(); i > 0; --i )
  {
    QModelIndex index = elems.at( i - 1 );
    if ( !index.isValid() || index.row() < 0 || index.row() >= static_cast<int>( melems.size() )
         || index.column() < 0 || index.column() > 3 )
      continue;

    BaseListElement* element = melems[ index.row() ];

    bool found = false;
    int id = 0;
    for ( std::vector<BaseListElement*>::iterator mit = melems.begin();
          mit != melems.end() && !found; )
    {
      if ( *mit == element )
      {
        found = true;
        beginRemoveRows( QModelIndex(), id, id );

        delete (*mit);
        mit = melems.erase( mit );
        element = 0;

        endRemoveRows();
      }
      else
        ++mit;
      ++id;
    }
  }
}

void TypesModel::clear()
{
  for ( std::vector<BaseListElement*>::const_iterator it = melems.begin();
          it != melems.end(); ++it )
    delete *it;
  melems.clear();
}

void TypesModel::elementChanged( const QModelIndex& index )
{
  if ( !index.isValid() || index.row() < 0 || index.row() >= static_cast<int>( melems.size() )
       || index.column() < 0 || index.column() > 3 )
    return;

  QModelIndex left = createIndex( index.row(), 1 );
  QModelIndex right = createIndex( index.row(), 2 );
  emit dataChanged( left, right );
}

bool TypesModel::isMacro( const QModelIndex& index ) const
{
  if ( !index.isValid() || index.row() < 0 || index.row() >= static_cast<int>( melems.size() ) )
    return false;

  return melems[ index.row() ]->isMacro();
}

Macro* TypesModel::macroFromIndex( const QModelIndex& index ) const
{
  if ( !index.isValid() || index.row() < 0 || index.row() >= static_cast<int>( melems.size() ) )
    return 0;

  BaseListElement* el = melems[ index.row() ];
  if ( !el->isMacro() )
    return 0;

  return static_cast<MacroListElement*>( el )->getMacro();
}

int TypesModel::columnCount( const QModelIndex& parent ) const
{
  return parent.isValid() ? 0 : 3;
}

QVariant TypesModel::data( const QModelIndex& index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( ( index.row() < 0 ) || ( index.row() >= static_cast<int>( melems.size() ) ) )
    return QVariant();

  switch ( role )
  {
    case Qt::DecorationRole:
    {
      if ( index.column() == 1 )
        return QIcon::fromTheme( melems[ index.row() ]->icon() );
      else
        return QVariant();
      break;
    }
    case Qt::DisplayRole:
    {
      switch ( index.column() )
      {
        case 0: return melems[ index.row() ]->type(); break;
        case 1: return melems[ index.row() ]->name(); break;
        case 2: return melems[ index.row() ]->description(); break;
        default:
          return QVariant();
      }
      break;
    }
    case Qt::ToolTipRole:
    {
      static QString macro_with_image(
          "<qt><table cellspacing=\"5\"><tr><td><b>%1</b> (%4)</td>"
          "<td rowspan=\"2\" align=\"right\"><img src=\"%3\"></td></tr>"
          "<tr><td>%2</td></tr></table></qt>" );
      static QString macro_no_image(
          QStringLiteral("<qt><b>%1</b> (%3)<br>%2</qt>") );

      if ( melems[ index.row() ]->icon( true ).isEmpty() )
        return macro_no_image
                 .arg( melems[ index.row() ]->name() )
                 .arg( wrapAt( melems[ index.row() ]->description() ) )
                 .arg( melems[ index.row() ]->type() );
      else
        return macro_with_image
                 .arg( melems[ index.row() ]->name() )
                 .arg( wrapAt( melems[ index.row() ]->description() ) )
                 .arg( KIconLoader::global()->iconPath( melems[ index.row() ]->icon(), - KIconLoader::SizeMedium ) )
                 .arg( melems[ index.row() ]->type() );
    }
    default:
      return QVariant();
  }
}

QVariant TypesModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation != Qt::Horizontal )
    return QVariant();

  if ( role == Qt::TextAlignmentRole )
    return QVariant( Qt::AlignLeft );

  if ( role != Qt::DisplayRole )
    return QVariant();

  switch ( section )
  {
    case 0: return i18n( "Type" ); break;
    case 1: return i18n( "Name" ); break;
    case 2: return i18n( "Description" ); break;
    default:
      return QVariant();
  }
}

QModelIndex TypesModel::index( int row, int column, const QModelIndex& parent ) const
{
  if ( parent.isValid() || row < 0 || row >= static_cast<int>( melems.size() ) || column < 0 || column > 3 )
    return QModelIndex();

  return createIndex( row, column, melems[row] );
}

int TypesModel::rowCount( const QModelIndex& parent ) const
{
  return parent.isValid() ? 0 : melems.size();
}


TypesDialog::TypesDialog( QWidget* parent, KigPart& part )
  : QDialog( parent ),
    mpart( part )
{
  setWindowTitle( i18nc("@title:window", "Manage Types") );
  QDialogButtonBox *buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help );
  QWidget *mainWidget = new QWidget( this );
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout( mainLayout );
  mainLayout->addWidget( mainWidget );
  QPushButton *okButton = buttonBox->button( QDialogButtonBox::Ok );
  okButton->setDefault( true );
  okButton->setShortcut( Qt::CTRL | Qt::Key_Return );
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
  mainLayout->addWidget( buttonBox );

  mtypeswidget = new Ui_TypesWidget();
  mtypeswidget->setupUi( mainWidget );
  mainWidget->layout()->setContentsMargins( 0, 0, 0, 0 );

  // model creation and usage
  mmodel = new TypesModel( mtypeswidget->typeList );
  mtypeswidget->typeList->setModel( mmodel );

  mtypeswidget->typeList->setContextMenuPolicy( Qt::CustomContextMenu );

  // improving GUI look'n'feel...
  mtypeswidget->buttonEdit->setIcon( QIcon::fromTheme( QStringLiteral("document-properties") ) );
  mtypeswidget->buttonRemove->setIcon( QIcon::fromTheme( QStringLiteral("edit-delete") ) );
  mtypeswidget->buttonExport->setIcon( QIcon::fromTheme( QStringLiteral("document-export") ) );
  mtypeswidget->buttonImport->setIcon( QIcon::fromTheme( QStringLiteral("document-import") ) );

  // loading macros...
  mmodel->addMacros( MacroList::instance()->macros() );

//  mtypeswidget->typeList->sortItems( 2, Qt::AscendingOrder );

  mtypeswidget->typeList->resizeColumnToContents( 0 );

  popup = new QMenu( this );
  popup->addAction( QIcon::fromTheme( QStringLiteral("document-properties") ), i18n( "&Edit..." ), this, &TypesDialog::editType );
  popup->addAction( QIcon::fromTheme( QStringLiteral("edit-delete") ), i18n( "&Delete" ), this, &TypesDialog::deleteType );
  popup->addSeparator();
  popup->addAction( QIcon::fromTheme( QStringLiteral("document-export") ), i18n( "E&xport..." ), this, &TypesDialog::exportType );

  // saving types
  mpart.saveTypes();

  connect( mtypeswidget->buttonExport, &QAbstractButton::clicked, this, &TypesDialog::exportType );
  connect( mtypeswidget->buttonImport, &QAbstractButton::clicked, this, &TypesDialog::importTypes );
  connect( mtypeswidget->buttonRemove, &QAbstractButton::clicked, this, &TypesDialog::deleteType );
  connect( mtypeswidget->buttonEdit, &QAbstractButton::clicked, this, &TypesDialog::editType );
  connect( mtypeswidget->typeList, &QWidget::customContextMenuRequested, this, &TypesDialog::typeListContextMenu );
  connect( buttonBox->button( QDialogButtonBox::Help ), &QAbstractButton::clicked, this, &TypesDialog::slotHelp );
  connect(okButton, &QAbstractButton::clicked, this, &TypesDialog::slotOk );
  connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &TypesDialog::slotCancel );

  resize( 460, 270 );
}

TypesDialog::~TypesDialog()
{
  delete mtypeswidget;
}

void TypesDialog::slotHelp()
{
  KHelpClient::invokeHelp( QStringLiteral("working-with-types"), QStringLiteral("kig") );
}

void TypesDialog::slotOk()
{
  mpart.saveTypes();
  mpart.deleteTypes();
  mpart.loadTypes();
  accept();
}

void TypesDialog::deleteType()
{
  std::vector<Macro*> selectedTypes;
  QModelIndexList indexes = selectedRows();
  for ( QModelIndexList::const_iterator it = indexes.constBegin(); it != indexes.constEnd(); ++it )
  {
    Macro* macro = mmodel->macroFromIndex( *it );
    if ( macro )
    {
      selectedTypes.push_back( macro );
    }
  }

  if (selectedTypes.empty()) return;
  QStringList types;
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j )
    types << ( *j )->action->descriptiveName();
  types.sort();
  if ( KMessageBox::warningContinueCancelList( this,
        i18np( "Are you sure you want to delete this type?",
              "Are you sure you want to delete these %1 types?", selectedTypes.size() ),
        types, i18n("Are You Sure?"), KStandardGuiItem::cont(), KStandardGuiItem::cancel(),
        QStringLiteral("deleteTypeWarning")) == KMessageBox::Cancel )
     return;
  bool updates = mtypeswidget->typeList->updatesEnabled();
  mtypeswidget->typeList->setUpdatesEnabled( false );
  mmodel->removeElements( indexes );
  mtypeswidget->typeList->setUpdatesEnabled( updates );
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  QModelIndexList indexes = selectedRows();
  for ( QModelIndexList::const_iterator it = indexes.constBegin(); it != indexes.constEnd(); ++it )
  {
    Macro* macro = mmodel->macroFromIndex( *it );
    if ( macro )
      types.push_back( macro );
  }
  if (types.empty()) return;
  QString file_name = QFileDialog::getSaveFileName( this, i18n( "Export Types" ), QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), i18n("Kig Types Files (*.kigt);;All Files (*)") );
  if ( file_name.isNull() )
    return;
  QFile fi( file_name );
  if ( fi.exists() )
    if ( KMessageBox::warningContinueCancel( this, i18n( "The file \"%1\" already exists. "
                                       "Do you wish to overwrite it?", fi.fileName() ),
                                       i18n( "Overwrite File?" ), KStandardGuiItem::overwrite() ) == KMessageBox::Cancel )
       return;
  MacroList::instance()->save( types, file_name );
}

void TypesDialog::importTypes()
{
  //TODO : Do this through MIME types
  QStringList toolFilters;
  toolFilters << i18n( "Kig Types Files (*.kigt)" );
#ifdef WITH_GEOGEBRA
  toolFilters << i18n( "Geogebra Tool Files (*.ggt)" );
#endif //WITH_GEOGEBRA
  toolFilters << i18n( "All Files (*)" );
  QStringList file_names = QFileDialog::getOpenFileNames( this, i18n( "Import Types" ), QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), toolFilters.join( QLatin1String( ";;" ) ) );

  std::vector<Macro*> macros;
  for ( QStringList::const_iterator i = file_names.constBegin();
        i != file_names.constEnd(); ++i)
  {
    std::vector<Macro*> nmacros;
#ifdef WITH_GEOGEBRA
    if( i->endsWith( QLatin1String( ".ggt" ) ) )// The input file is a Geogebra Tool file..
    {
      loadGeogebraTools( *i, macros, mpart );
      continue;
    }
#endif //WITH_GEOGEBRA
    bool ok = MacroList::instance()->load( *i, nmacros, mpart );
    if ( ! ok )
      continue;
    std::copy( nmacros.begin(), nmacros.end(), std::back_inserter( macros ) );
  };
  MacroList::instance()->add( macros );

  mmodel->addMacros( macros );

  mtypeswidget->typeList->resizeColumnToContents( 0 );
}

void TypesDialog::editType()
{
  QModelIndexList indexes = selectedRows();
  if ( indexes.isEmpty() )
    return;
  if ( indexes.count() > 1 )
  {
    KMessageBox::sorry( this,
                        i18n( "There is more than one type selected. You can "
                              "only edit one type at a time. Please select "
                              "only the type you want to edit and try again." ),
                        i18n( "More Than One Type Selected" ) );
    return;
  }
  bool refresh = false;
  QModelIndex index = indexes.first();
  if ( mmodel->isMacro( index ) )
  {
    Macro* oldmacro = mmodel->macroFromIndex( index );
    EditType editdialog( this, oldmacro->action->descriptiveName(), oldmacro->action->description(),
                         oldmacro->ctor->iconFileName( false ) );
    if ( editdialog.exec() )
    {
      QString newname = editdialog.name();
      QString newdesc = editdialog.description();
      QString newicon = editdialog.icon();

//      mpart.unplugActionLists();
      oldmacro->ctor->setName( newname );
      oldmacro->ctor->setDescription( newdesc );
      QByteArray ncicon( newicon.toUtf8() );
      oldmacro->ctor->setIcon( ncicon );
//      mpart.plugActionLists();
      refresh = true;
    }
  }
  if ( refresh )
  {
    mmodel->elementChanged( index );
  }
}

void TypesDialog::slotCancel()
{
  mpart.deleteTypes();
  mpart.loadTypes();
  reject();
}

QModelIndexList TypesDialog::selectedRows() const
{
  QModelIndexList indexes = mtypeswidget->typeList->selectionModel()->selectedRows();
  std::sort( indexes.begin(), indexes.end() );
  return indexes;
}

void TypesDialog::typeListContextMenu( const QPoint& pos )
{
  QModelIndexList indexes = mtypeswidget->typeList->selectionModel()->selectedRows();
  if ( indexes.isEmpty() )
    return;

  popup->exec( mtypeswidget->typeList->viewport()->mapToGlobal( pos ) );
}

#ifdef WITH_GEOGEBRA
bool TypesDialog::loadGeogebraTools( const QString& sFrom, std::vector<Macro*>& vec, KigPart& /*kigpart*/ )
{
  KZip geogebraFile( sFrom );

  if ( geogebraFile.open( QIODevice::ReadOnly ) )
  {
    const KZipFileEntry* geogebraXMLEntry = dynamic_cast<const KZipFileEntry*>( geogebraFile.directory()->entry( QStringLiteral("geogebra_macro.xml") ) );

    if ( geogebraXMLEntry )
    {
      KigDocument * document = new KigDocument();
      QXmlNamePool np;
      QXmlQuery geogebraXSLT( QXmlQuery::XSLT20, np );
      const QString encodedData = QString::fromUtf8( geogebraXMLEntry->data().constData() );
      QFile queryDevice( QStringLiteral(":/kig/geogebra/geogebra.xsl") );
      GeogebraTransformer ggttransformer( document, np );

      queryDevice.open( QFile::ReadOnly );
      geogebraXSLT.setFocus( encodedData );
      geogebraXSLT.setQuery( &queryDevice );
      geogebraXSLT.evaluateTo( &ggttransformer );
      queryDevice.close();

      const size_t nmacros = ggttransformer.getNumberOfSections();

      for( size_t i = 0; i < nmacros; i++ )
      {
        const GeogebraSection f = ggttransformer.getSection( i );
        ObjectHierarchy hrchy( f.getInputObjects(), f.getOutputObjects() );
        MacroConstructor* ctor = new MacroConstructor( hrchy, ggttransformer.getSection( i ).getName(), ggttransformer.getSection( i ).getDescription() );
        ConstructibleAction* act = new ConstructibleAction( ctor, 0 );

        Macro* newmacro = new Macro( act, ctor );
        vec.push_back( newmacro );
      }
    }
  }
  else
  {
    qWarning() << "Failed to open zip archive";
    return false;
  }

  return true;
}
#endif //WITH_GEOEBRA
