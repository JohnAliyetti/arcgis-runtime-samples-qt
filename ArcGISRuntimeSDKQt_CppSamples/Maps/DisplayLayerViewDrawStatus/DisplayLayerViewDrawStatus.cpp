// [WriteFile Name=DisplayLayerViewDrawStatus, Category=Maps]
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

#include "DisplayLayerViewDrawStatus.h"

#include "ArcGISMapImageLayer.h"
#include "ArcGISTiledLayer.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "Point.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

DisplayLayerViewDrawStatus::DisplayLayerViewDrawStatus(QQuickItem* parent) :
  QQuickItem(parent)
{
}

DisplayLayerViewDrawStatus::~DisplayLayerViewDrawStatus()
{
}

void DisplayLayerViewDrawStatus::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayLayerViewDrawStatus>("Esri.Samples", 1, 0, "DisplayLayerViewDrawStatusSample");
}

void DisplayLayerViewDrawStatus::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new map instance without a basemap
  m_map = new Map(this);

  // add three layers to map and connect signal
  addLayers();

  // set map on the map view
  m_mapView->setMap(m_map);

  // set Initial Viewpoint
  m_mapView->setViewpointCenter(Point(-11e6, 45e5, SpatialReference(102100)), 5e7);

  // initialize QStringList of layer names and states
  for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
  {
    m_layerNames.append(m_map->operationalLayers()->at(i)->name());
    m_layerViewStates.append(QString("Unknown"));
  }

  emit namesChanged();

  connectSignals();
}

//add layers and connect layerViewStateChanged signal
void DisplayLayerViewDrawStatus::addLayers()
{
  // create tiled layer using a url
  m_tiledLayer = new ArcGISTiledLayer(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer"), this);
  // add to map
  m_map->operationalLayers()->append(m_tiledLayer);

  // create map image using url
  m_imageLayer = new ArcGISMapImageLayer(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);
  m_imageLayer->setMinScale(40000000);
  m_imageLayer->setMaxScale(2000000);
  // add to map
  m_map->operationalLayers()->append(m_imageLayer);

  // create feature table using url
  m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"), this);
  // create feature layer using table
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  // add feature layer to map
  m_map->operationalLayers()->append(m_featureLayer);
}

void DisplayLayerViewDrawStatus::connectSignals()
{
  // connect layerViewStateChanged signal
  connect(m_mapView, &MapQuickView::layerViewStateChanged, this, [this](Layer* layer, LayerViewState viewState)
  {
    int rIndex = 0;

    // find index in QStringList of that layer
    for (int i = 0; i < m_map->operationalLayers()->size(); ++i)
    {
      if (layer == m_map->operationalLayers()->at(i))
        rIndex = i;
    }

    // replace layer name in QStringList
    m_layerNames[rIndex] = layer->name();

    // use insert to replace values mapped to layer name
    if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Active)
      m_layerViewStates[rIndex] = QString("Active");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::NotVisible)
      m_layerViewStates[rIndex] = QString("Not Visible");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::OutOfScale)
      m_layerViewStates[rIndex] = QString("Out of Scale");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Loading)
      m_layerViewStates[rIndex] = QString("Loading");
    else if (viewState.statusFlags() & Esri::ArcGISRuntime::LayerViewStatus::Error)
      m_layerViewStates[rIndex] = QString("Error");
    else
      m_layerViewStates[rIndex] = QString("Unknown");

    emit statusChanged();
  });

  connect(m_map, &Map::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loaded)
      emit mapReady();
  });
}

QStringList DisplayLayerViewDrawStatus::layerViewStates() const
{
  return m_layerViewStates;
}

QStringList DisplayLayerViewDrawStatus::layerNames() const
{
  return m_layerNames;
}

