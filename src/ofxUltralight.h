#pragma once

#include <ofMain.h>
#include <Ultralight/Ultralight.h>
#include <JavaScriptCore/JavaScript.h>
#include <AppCore/Platform.h>
#include <AppCore/JSHelpers.h>
//#include <opencv.hpp>

namespace ofxUL {

using namespace ultralight;

// Define our custom Logger class
class MyLogger : public ultralight::Logger {
public:
    MyLogger() {}
    virtual ~MyLogger() {}
    /// Called when the library wants to print a message to the log.
    virtual void LogMessage(LogLevel log_level, const String16& message) override {
        // give out the message via OF Logging
        if (log_level == kLogLevel_Error) {
            ofLogError( ofToString("[Ultralight]: ") + String(message).utf8().data() );
        } else if (log_level == kLogLevel_Warning) {
            ofLogWarning(ofToString("[Ultralight]: ") + String(message).utf8().data() );
        } else {
            ofLogNotice(ofToString("[Ultralight]: ") + String(message).utf8().data() );
        }
        
    }
    
    inline std::string ToUTF8(const String& str) {
        String8 utf8 = str.utf8();
        return std::string(utf8.data(), utf8.length());
    }
    
    inline const char* Stringify(MessageSource source) {
        switch(source) {
            case kMessageSource_XML: return "XML";
            case kMessageSource_JS: return "JS";
            case kMessageSource_Network: return "Network";
            case kMessageSource_ConsoleAPI: return "ConsoleAPI";
            case kMessageSource_Storage: return "Storage";
            case kMessageSource_AppCache: return "AppCache";
            case kMessageSource_Rendering: return "Rendering";
            case kMessageSource_CSS: return "CSS";
            case kMessageSource_Security: return "Security";
            case kMessageSource_ContentBlocker: return "ContentBlocker";
            case kMessageSource_Other: return "Other";
            default: return "";
        }
    }
    
    inline const char* Stringify(MessageLevel level) {
        switch(level) {
            case kMessageLevel_Log: return "Log";
            case kMessageLevel_Warning: return "Warning";
            case kMessageLevel_Error: return "Error";
            case kMessageLevel_Debug: return "Debug";
            case kMessageLevel_Info: return "Info";
            default: return "";
        }
    }
    
    
};

class ViewAsset : public LoadListener, public ViewListener {
public:
    ViewAsset(Ref<View> view) : view(view) {}
    
    void load(int width, int height, ofVec2f t_offset, string url);
    virtual void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,const String& url);
    void update();
    void draw();
    void keyPressed(int key);
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    
    ofTexture& getTexture() {
        return oeTexture;
    }
    const ofTexture& getTexture() const {
        return oeTexture;
    }
    ofPixels& getPixels() {
        return pixels;
    }
    const ofPixels& getPixels() const {
        return pixels;
    }
    
    bool DOMready = false;
    bool needs_redraw = false;
    
private:
    RefPtr<View> view, inspectorView;
    ofTexture oeTexture, inspectorTexture;
    JSContextRef jsContext;
    ofVec2f offset;
    ofPixels pixels;
    //    cv::Mat mat_rgba;
    //    cv::Mat mat_bgr;
    //    GLuint pbo_id[2];
};

class ofxUltralight
{
    
public:
    
    
    inline std::string ToUTF8(const String& str) {
        String8 utf8 = str.utf8();
        return std::string(utf8.data(), utf8.length());
    }
    
    inline const char* Stringify(MessageSource source) {
        switch (source) {
            case kMessageSource_XML: return "XML";
            case kMessageSource_JS: return "JS";
            case kMessageSource_Network: return "Network";
            case kMessageSource_ConsoleAPI: return "ConsoleAPI";
            case kMessageSource_Storage: return "Storage";
            case kMessageSource_AppCache: return "AppCache";
            case kMessageSource_Rendering: return "Rendering";
            case kMessageSource_CSS: return "CSS";
            case kMessageSource_Security: return "Security";
            case kMessageSource_ContentBlocker: return "ContentBlocker";
            case kMessageSource_Other: return "Other";
            default: return "";
        }
    }
    
    inline const char* Stringify(MessageLevel level) {
        switch (level) {
            case kMessageLevel_Log: return "Log";
            case kMessageLevel_Warning: return "Warning";
            case kMessageLevel_Error: return "Error";
            case kMessageLevel_Debug: return "Debug";
            case kMessageLevel_Info: return "Info";
            default: return "";
        }
    }
    
    void OnAddConsoleMessage(View* caller,
                             MessageSource source,
                             MessageLevel level,
                             const String& message,
                             uint32_t line_number,
                             uint32_t column_number,
                             const String& source_id);
    
    void setup();
    int load(int width, int height, string url);
    int load(int width, int height, ofVec2f t_offset, string url);
    void update();
    void draw();
    void draw(int i);
    void unload(int i);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    string getStringFromJSstr(JSString str);
    
    ofTexture& getTexture(int i) {
        return assets[i]->getTexture();
    }
    const ofTexture& getTexture(int i) const {
        return assets[i]->getTexture();
    }
    ofPixels& getPixels(int i) {
        return assets[i]->getPixels();
    }
    const ofPixels& getPixels(int i) const {
        return assets[i]->getPixels();
    }
    
    Config config;
    shared_ptr<GPUDriver> gpu_driver;
    RefPtr<Renderer> renderer;
    vector<ViewAsset*> assets;
};

}
