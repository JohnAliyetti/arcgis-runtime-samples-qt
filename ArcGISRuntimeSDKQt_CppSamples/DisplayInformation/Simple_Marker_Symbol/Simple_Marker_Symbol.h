// [WriteFile Name=Simple_Marker_Symbol, Category=DisplayInformation]
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

#ifndef SIMPLE_MARKER_SYMBOL_H
#define SIMPLE_MARKER_SYMBOL_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class GraphicsOverlay;
    }
}

#include <QQuickItem>

class Simple_Marker_Symbol : public QQuickItem
{
    Q_OBJECT

public:
    Simple_Marker_Symbol(QQuickItem* parent = nullptr);
    ~Simple_Marker_Symbol();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay;
};

#endif // SIMPLE_MARKER_SYMBOL_H
