# ofxImGuiJson

**ofxImGuiJson** is an openFrameworks addon that automatically generates ImGui interfaces based on JSON schemas. Simply define a JSON schema and the corresponding editable UI will be created automatically.

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![openFrameworks](https://img.shields.io/badge/openFrameworks-0.12+-brightgreen.svg)](https://openframeworks.cc/)

## Features

- **Schema-based UI Generation**: Automatic UI generation based on JSON schemas
- **Rich Data Type Support**: boolean, int, float, string, object, array, numeric arrays
- **Diverse UI Widgets**: slider, drag, input, table, chips, collapsible, etc.
- **Path-based Callbacks**: Monitor changes to specific data paths
- **Template-based Design**: Compatible with various JSON libraries including nlohmann::json
- **Constraint Support**: Detailed control with minimum, maximum, speed parameters

## Setup

### Dependencies

- [ofxImGui](https://github.com/jvcleave/ofxImGui) - openFrameworks version of ImGui

### Installation

1. Clone into openFrameworks `addons` folder:
```bash
cd OF_ROOT/addons
git clone https://github.com/nariakiiwatani/ofxImGuiJson.git
```

2. Add to your project's `addons.make`:
```
ofxImGui
ofxImGuiJson
```

3. Include headers:
```cpp
#include "ofxImGui.h"
#include "ofxImGuiJson.h"
#include <nlohmann/json.hpp>
```

## Basic Usage

### Quick Start

```cpp
#include "ofApp.h"

class ofApp : public ofBaseApp {
    ofxImGui::Gui gui;
    nlohmann::json schema;
    nlohmann::json data;
    
public:
    void setup() override {
        gui.setup();
        
        // Define schema
        schema = {
            {"type", "object"},
            {"properties", {
                {"name", {{"type", "string"}, {"default", "John"}}},
                {"age", {{"type", "int"}, {"minimum", 0}, {"maximum", 100}, {"default", 25}}}
            }}
        };
        
        // Generate default data from schema
        data = ofxImGuiJson::GenerateDefault(schema);
    }
    
    void draw() override {
        gui.begin();
        if (ImGui::Begin("Settings")) {
            // Edit JSON data
            ofxImGuiJson::Edit("Profile", data, &schema);
        }
        ImGui::End();
        gui.end();
    }
};
```

## Architecture

### Component Structure

```
ofxImGuiJson/
├── src/
│   ├── ofxImGuiJson.h              # Main API
│   ├── ofxImGuiJsonDispatcher.h     # Editor dispatcher
│   ├── ofxImGuiJsonCallback.h       # Callback system
│   └── Editors/
│       ├── ofxImGuiJsonPrimitiveEditor.h    # Primitive type editor
│       ├── ofxImGuiJsonObjectEditor.h       # Object type editor
│       ├── ofxImGuiJsonArrayEditor.h        # Array type editor
│       └── ofxImGuiJsonNumericArrayEditor.h # Numeric array editor
```

### Data Flow

1. **Schema Analysis**: Extract type information from JSON schema
2. **Dispatch**: Select appropriate editor based on data type
3. **UI Generation**: Generate corresponding ImGui components
4. **Change Detection**: Monitor value changes from user input
5. **Callback Execution**: Execute callbacks corresponding to changed paths

## API Reference

### Main Functions

#### `Edit()`

```cpp
template<typename JsonType>
bool Edit(const char *label, JsonType &data, const JsonType *schema,
          const CallbackList<JsonType> &callbacks = {})
```

Main function to edit JSON data.

**Parameters:**
- `label`: UI label
- `data`: JSON data to edit (reference)
- `schema`: JSON schema (pointer)
- `callbacks`: Callback list (optional)

**Returns:**
- `bool`: `true` if data was changed

#### `GenerateDefault()`

```cpp
template<typename JsonType>
JsonType GenerateDefault(const JsonType &schema)
```

Generate default data from schema.

**Parameters:**
- `schema`: JSON schema

**Returns:**
- `JsonType`: Generated default data

## Schema Specification

### Basic Structure

```json
{
  "type": "data_type",
  "label": "UI_display_name",
  "default": "default_value",
  "ui:widget": "widget_type"
}
```

### Supported Types

#### 1. Primitive Types

##### Boolean
```json
{
  "type": "boolean",
  "label": "Enable Feature",
  "default": true
}
```

##### Integer
```json
{
  "type": "int",
  "label": "Age",
  "minimum": 0,
  "maximum": 120,
  "default": 25,
  "ui:widget": "slider" // "drag", "input"
}
```

##### Float
```json
{
  "type": "float",
  "label": "Height",
  "minimum": 0.0,
  "maximum": 250.0,
  "speed": 0.1,
  "default": 170.5,
  "ui:widget": "drag" // "slider", "input"
}
```

##### String
```json
{
  "type": "string",
  "label": "Name",
  "default": "John Doe"
}
```

#### 2. Object Type

```json
{
  "type": "object",
  "label": "Profile",
  "ui:widget": "collapsible", // "inline"
  "properties": {
    "name": {"type": "string", "default": "John"},
    "age": {"type": "int", "default": 25}
  }
}
```

**Widgets:**
- `"collapsible"`: Collapsible tree node
- `"inline"`: Inline display

#### 3. Array Type

```json
{
  "type": "array",
  "label": "Items",
  "ui:widget": "table", // "chips"
  "items": {"type": "string", "default": "item"},
  "minItems": 0,
  "maxItems": 10,
  "default": ["item1", "item2"]
}
```

**Widgets:**
- `"table"`: Table format display
- `"chips"`: Chip format display (recommended for string arrays)

#### 4. Numeric Array Type

Fixed-size numeric arrays (for vectors, coordinates, etc.):

```json
{
  "type": "float", // or "int"
  "label": "RGB Color",
  "size": 3,
  "minimum": 0.0,
  "maximum": 1.0,
  "ui:widget": "slider", // "drag", "input"
  "default": [0.5, 0.5, 0.5]
}
```

## Callback System

### Basic Usage

```cpp
using namespace ofxImGuiJson;

CallbackList<nlohmann::json> callbacks = {
    // Path-specific callback
    {{"age"}, [](const std::string& path, const auto& oldVal, const auto& newVal) {
        ofLogNotice("Changed") << "Path: " << path << ", New value: " << newVal;
    }},
    
    // Wildcard pattern
    {{"profile/*"}, [](const auto& newVal) {
        ofLogNotice("Profile") << "Profile changed: " << newVal;
    }},
    
    // Regular expression pattern
    {{std::regex(R"(scores\[\d+\])")}, []() {
        ofLogNotice("Scores") << "Score array updated";
    }}
};

// Edit with callbacks
ofxImGuiJson::Edit("Settings", data, &schema, callbacks);
```

### Path Patterns

1. **Exact Match**: `"name"` - Matches exact path
2. **Wildcard**: `"profile/*"` - Pattern matching
3. **Regular Expression**: `std::regex(pattern)` - Advanced pattern matching

### Callback Function Formats

Supports various argument patterns:

```cpp
// Full arguments
[](const std::string& path, const auto& oldVal, const auto& newVal) { ... }

// Path and new value only
[](const std::string& path, const auto& newVal) { ... }

// Path only
[](const std::string& path) { ... }

// Value change only
[](const auto& oldVal, const auto& newVal) { ... }

// New value only
[](const auto& newVal) { ... }

// No arguments
[]() { ... }
```

## Practical Examples

### Game Settings UI

```cpp
nlohmann::json gameSettings = {
    {"type", "object"},
    {"label", "Game Settings"},
    {"ui:widget", "collapsible"},
    {"properties", {
        {"graphics", {
            {"type", "object"},
            {"label", "Graphics"},
            {"properties", {
                {"resolution", {
                    {"type", "int"},
                    {"size", 2},
                    {"label", "Resolution"},
                    {"default", {1920, 1080}}
                }},
                {"vsync", {
                    {"type", "boolean"},
                    {"label", "V-Sync"},
                    {"default", true}
                }}
            }}
        }},
        {"audio", {
            {"type", "object"},
            {"label", "Audio"},
            {"properties", {
                {"masterVolume", {
                    {"type", "float"},
                    {"label", "Master Volume"},
                    {"ui:widget", "slider"},
                    {"minimum", 0.0},
                    {"maximum", 1.0},
                    {"default", 0.8}
                }}
            }}
        }}
    }}
};

// Usage example
data = ofxImGuiJson::GenerateDefault(gameSettings);

// Monitor settings changes
CallbackList<nlohmann::json> settingsCallbacks = {
    {{"graphics/vsync"}, [this](bool enabled) {
        ofSetVerticalSync(enabled);
    }},
    {{"audio/masterVolume"}, [this](float volume) {
        soundManager.setMasterVolume(volume);
    }}
};

// UI rendering
ofxImGuiJson::Edit("Settings", data, &gameSettings, settingsCallbacks);
```

### Array Data Editing

```cpp
nlohmann::json arraySchema = {
    {"type", "array"},
    {"label", "Enemy Spawn Points"},
    {"ui:widget", "table"},
    {"items", {
        {"type", "object"},
        {"properties", {
            {"position", {
                {"type", "float"},
                {"size", 2},
                {"label", "Position"},
                {"default", {0.0, 0.0}}
            }},
            {"enemyType", {
                {"type", "string"},
                {"label", "Enemy Type"},
                {"default", "basic"}
            }}
        }}
    }},
    {"minItems", 1},
    {"maxItems", 20}
};
```

## Advanced Features

### Custom Schema Extensions

Add custom UI control properties:

```json
{
  "type": "float",
  "label": "Speed",
  "ui:widget": "drag",
  "ui:tooltip": "Movement speed in units per second",
  "minimum": 0.0,
  "maximum": 10.0,
  "speed": 0.1,
  "default": 5.0
}
```

## License

MIT License

## Contributing

Pull requests and issue reports are welcome.

## Related Projects

- [ofxImGui](https://github.com/jvcleave/ofxImGui) - ImGui for openFrameworks
- [nlohmann/json](https://github.com/nlohmann/json) - Modern C++ JSON Library
- [JSON Schema](https://json-schema.org/) - JSON Schema Specification