// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "UpdateAttributesFeatureService.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Feature.h"
#include "ArcGISFeature.h"
#include "FeatureEditResult.h"
#include "FeatureQueryResult.h"
#include <QUrl>
#include <QUuid>
#include <QMouseEvent>

using namespace Esri::ArcGISRuntime;

UpdateAttributesFeatureService::UpdateAttributesFeatureService(QQuickItem* parent) :
  QQuickItem(parent)
{    
}

UpdateAttributesFeatureService::~UpdateAttributesFeatureService()
{
}

void UpdateAttributesFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<UpdateAttributesFeatureService>("Esri.Samples", 1, 0, "UpdateAttributesFeatureServiceSample");
}

void UpdateAttributesFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // create a Map by passing in the Basemap
  m_map = new Map(Basemap::streets(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-10800000, 4500000, SpatialReference(102100)), 3e7));

  // set map on the map view
  m_mapView->setMap(m_map);

  // create the ServiceFeatureTable
  m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

  // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  m_featureLayer->setSelectionWidth(3);
  m_map->operationalLayers()->append(m_featureLayer);

  connectSignals();
}

void UpdateAttributesFeatureService::connectSignals()
{   
  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // first clear the selection
    m_featureLayer->clearSelection();

    // set the properties for qml
    m_screenX = mouseEvent.x();
    emit screenXChanged();
    m_screenY = mouseEvent.y();
    emit screenYChanged();
    emit hideWindow();

    // call identify on the map view
    m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
  });

  // connect to the viewpoint changed signal on the MapQuickView
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    m_featureLayer->clearSelection();
    emit hideWindow();
  });

  // connect to the identifyLayerCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyLayerCompleted, this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if(!identifyResult)
      return;
    if (!identifyResult->geoElements().empty())
    {
      // select the item in the result
      m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));

      // obtain the selected feature with attributes
      QueryParameters queryParams;
      QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
      queryParams.setWhereClause(whereClause);
      m_featureTable->queryFeatures(queryParams);
    }
  });

  // connect to the queryFeaturesCompleted signal on the feature table
  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    if (featureQueryResult && featureQueryResult->iterator().hasNext())
    {
      // first delete if not nullptr
      if (m_selectedFeature != nullptr)
        delete m_selectedFeature;

      // set selected feature member
      m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
      m_featureType = m_selectedFeature->attributes()->attributeValue("typdamage").toString();
      emit featureTypeChanged();
      emit featureSelected();
    }
  });

  // connect to the updateFeatureCompleted signal to determine if the update was successful
  connect(m_featureTable, &ServiceFeatureTable::updateFeatureCompleted, this, [this](QUuid, bool success)
  {
    // if the update was successful, call apply edits to apply to the service
    if (success)
      m_featureTable->applyEdits();
  });

  // connect to the applyEditsCompleted signal from the ServiceFeatureTable
  connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, this, [this](QUuid, const QList<FeatureEditResult*>& featureEditResults)
  {
    // check if result list is not empty
    if (!featureEditResults.isEmpty())
    {
      // obtain the first item in the list
      auto featureEditResult = featureEditResults.first();
      // check if there were errors, and if not, log the new object ID
      if (!featureEditResult->isCompletedWithErrors())
        qDebug() << "Successfully updated attribute for Object ID:" << featureEditResult->objectId();
      else
        qDebug() << "Apply edits error.";
    }
    m_featureLayer->clearSelection();
  });
}

void UpdateAttributesFeatureService::updateSelectedFeature(QString fieldVal)
{
  // connect to load status changed signal
  connect(m_selectedFeature, &ArcGISFeature::loadStatusChanged,
          this, [this, fieldVal](Esri::ArcGISRuntime::LoadStatus)
  {
    if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
    {
      disconnect(m_selectedFeature, &ArcGISFeature::loadStatusChanged, 0, 0); // bad...

      // update the select feature's attribute value
      m_selectedFeature->attributes()->replaceAttribute("typdamage", fieldVal);

      // update the feature in the feature table
      m_featureTable->updateFeature(m_selectedFeature);
    }
  });

  // load selecte feature
  m_selectedFeature->load();
}

int UpdateAttributesFeatureService::screenX() const
{
  return m_screenX;
}

int UpdateAttributesFeatureService::screenY() const
{
  return m_screenY;
}

QString UpdateAttributesFeatureService::featureType() const
{
  return m_featureType;
}
