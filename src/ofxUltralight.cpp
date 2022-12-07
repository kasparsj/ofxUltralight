#include "ofxUltralight.h"

using namespace ofxUltralight;

void ViewAsset::load(int width, int height, ofVec2f t_offset, string url) {
    offset = t_offset;
    
    //inspectorView = view->inspector();
    //inspectorView->Resize(700, 300);
    //string ipath = ofToDataPath("", true) + "/inspector/Main.html";
    //inspectorView->LoadURL( ipath.c_str() );

    bool isURL = false;
    // there seem to be problem with colons, so some workaround. Feel free to fix it...
    if (url.rfind("http", 0) == 0 && url.rfind("//", 6)) {
        ofLogVerbose("starts with http:// ... set it as a URI");
        isURL = true;
    }
    if (url.rfind("https", 0) == 0 && url.rfind("//", 7)) {
        ofLogVerbose("starts with https:// ... set it as a URI");
        isURL = true;
    }
    if (url.rfind("file", 0) == 0 && url.rfind("///", 6)) {
        ofLogVerbose("starts with file:/// ... set it as a URI");
        isURL = true;
    }

    if (isURL) {
        view->LoadURL(url.c_str());
    }
    else {
        ofLogVerbose("maybe it's still a File - try to load it");
        ofBuffer buffer = ofBufferFromFile(url);
        string content;
        if (buffer.size()) {
            for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {

                content += *it;

            }
            view->LoadHTML(content.c_str());
        }
        else {
            view->LoadHTML( url.c_str() );
        }
    }
    view->Focus();
    view->set_load_listener(this);
    view->set_view_listener(this);

}

void ViewAsset::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
    ofLogVerbose( "Ultralight DOM ready" );
    DOMready = true;

    Ref<JSContext> locked_context = caller->LockJSContext();
    SetJSContext(locked_context.get());
}

void ViewAsset::update() {
    needs_redraw = view->needs_paint();
}

void ViewAsset::draw() {
    ///
    /// Cast it to a BitmapSurface.
    ///
    BitmapSurface* bitmap_surface = (BitmapSurface*)(view->surface());
    if ( !bitmap_surface->dirty_bounds().IsEmpty() && !view->is_loading() ) {
        //ofLogVerbose("dirty, so draw");

        ///
    /// Get the underlying bitmap.
    ///
        RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();

        void* pixels1 = bitmap->LockPixels();

        /// Get the bitmap dimensions.
        uint32_t width = bitmap->width();
        uint32_t height = bitmap->height();
        uint32_t stride = bitmap->row_bytes();

        unsigned char* pixels2 = (unsigned char*)pixels1;
        pixels.setFromExternalPixels(pixels2, width, height, OF_PIXELS_BGRA);
        // load the pixels to ofTexture
        oeTexture.loadData(pixels);

        /// Unlock the Bitmap when we are done.
        bitmap->UnlockPixels();

        /// Clear the dirty bounds.
        bitmap_surface->ClearDirtyBounds();
        
    }
    oeTexture.draw(offset.x, offset.y);
}

void ViewAsset::keyPressed(int key) {
    int text = key;
    // get the correct virtual key codes
    int vk = key;
    switch (key) {
        case OF_KEY_LEFT:
            vk = KeyCodes::GK_LEFT;
            break;
        case OF_KEY_RIGHT:
            vk = KeyCodes::GK_RIGHT;
            break;
        case OF_KEY_UP:
            vk = KeyCodes::GK_UP;
            break;
        case OF_KEY_DOWN:
            vk = KeyCodes::GK_DOWN;
            break;
        case OF_KEY_DEL:
            vk = KeyCodes::GK_DELETE;
            break;
        case OF_KEY_BACKSPACE:
            vk = KeyCodes::GK_BACK;
            break;
            // this is the 'dot' key. For some reason I had to hard-code this in order to make it work
        case 46:
            vk = KeyCodes::GK_DECIMAL;
            break;
        case -1:
            break;

        case OF_KEY_RETURN:
            vk = KeyCodes::GK_RETURN;
            return;
        case OF_KEY_LEFT_SHIFT:
            vk = KeyCodes::GK_LSHIFT;
            return;
        case OF_KEY_RIGHT_SHIFT:
            vk = KeyCodes::GK_RSHIFT;
            return;
        case OF_KEY_LEFT_CONTROL:
            vk = KeyCodes::GK_LCONTROL;
            return;
        case OF_KEY_RIGHT_CONTROL:
            vk = KeyCodes::GK_RCONTROL;
            return;
        case OF_KEY_LEFT_ALT:
            return;
        case OF_KEY_RIGHT_ALT:
            return;
        case OF_KEY_LEFT_COMMAND:
            return;
        case OF_KEY_RIGHT_COMMAND:
            return;
        default:
            break;
    }

    KeyEvent evt;
    evt.type = KeyEvent::kType_RawKeyDown;
    // You'll need to generate a key identifier from the virtual key code
    // when synthesizing events. This function is provided in KeyEvent.h
    evt.virtual_key_code = vk;
    evt.native_key_code = key;
    evt.text = ofToString( (char)text ).c_str();
    evt.unmodified_text = ofToString( (char)text ).c_str();
    //evt.modifiers = (char)OF_KEY_MODIFIER;
    GetKeyIdentifierFromVirtualKeyCode(evt.virtual_key_code, evt.key_identifier);
    view->FireKeyEvent(evt);

    KeyEvent evt2;
    evt2.type = KeyEvent::kType_Char;
    evt2.virtual_key_code = vk;
    evt2.native_key_code = key;
    evt2.text = ofToString( (char)text ).c_str();
    evt2.unmodified_text = ofToString( (char)text ).c_str();
    //evt2.modifiers = (char)OF_KEY_MODIFIER;
    evt2.unmodified_text = ofToString( (char)key ).c_str(); // If not available, set to same as evt.text

    view->FireKeyEvent(evt2);
}

void ViewAsset::mouseMoved(int x, int y) {
    MouseEvent evt;
    evt.type = MouseEvent::kType_MouseMoved;
    evt.x = x - offset.x;
    evt.y = y - offset.y;
    evt.button = MouseEvent::kButton_None;

    view->FireMouseEvent(evt);
}

void ViewAsset::mousePressed(int x, int y, int button) {
    //cout << "Klick Mouse at " << x - offset.x << " - " << y - offset.y ;
    MouseEvent evt;
    evt.type = MouseEvent::kType_MouseDown;
    evt.x = x - offset.x;
    evt.y = y - offset.y;
    evt.button = MouseEvent::kButton_Left;

    view->FireMouseEvent(evt);
}

void ViewAsset::mouseReleased(int x, int y, int button) {
    MouseEvent evt;
    evt.type = MouseEvent::kType_MouseUp;
    evt.x = x - offset.x;
    evt.y = y - offset.y;
    evt.button = MouseEvent::kButton_Left;

    view->FireMouseEvent(evt);
}

void ViewAsset::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    ScrollEvent scroll_event;
    scroll_event.type = ScrollEvent::kType_ScrollByPixel;
    scroll_event.delta_x = scrollX * 30;
    scroll_event.delta_y = scrollY * 30;

    view->FireScrollEvent(scroll_event);
}

Manager* Manager::instance = NULL;

bool Manager::setup() {
    if (instance != NULL) {
        return false;
    }

    config.resource_path = ofToDataPath("resources").c_str();
	config.use_gpu_renderer = false;
	config.device_scale = 1.0;
	config.user_agent = "Mozilla/5.0 (Linux; Android 8.1.0; SM-G965F Build/OPM2.171019.029) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/7.2 Chrome/59.0.3071.125 Mobile Safari/537.36";

	auto& platform = Platform::instance();
	platform.set_font_loader(GetPlatformFontLoader());
	platform.set_config(config);
	platform.set_logger(new MyLogger());
	platform.set_file_system(GetPlatformFileSystem("data"));

	//gpu_driver = make_shared<GPUDriverGL>(1);
	//platform.set_gpu_driver(gpu_driver.get());

	renderer = Renderer::Create();
    
    instance = this;
    
    return true;
}

ViewAsset* Manager::createView(int width, int height, string url) {
    return createView(width, height, ofVec2f(0), url);
}

//--------------------------------------------------------------
ViewAsset* Manager::createView(int width, int height, ofVec2f t_offset, string url) {
    ViewAsset* asset = new ViewAsset(renderer->CreateView(width, height, false, nullptr));
    assets[asset->getId()] = asset;
    asset->load(width, height, t_offset, url);
    return asset;
}

//--------------------------------------------------------------
void Manager::OnAddConsoleMessage(View* caller,
	MessageSource source,
	MessageLevel level,
	const String& message,
	uint32_t line_number,
	uint32_t column_number,
	const String& source_id) {
	
	// give out the console Message via OF Logging
	auto getMessage = (string) Stringify(source);
	ofLogNotice( "[Ultralight: Console]: [" + getMessage + "] [" + Stringify(level) + "] " + ToUTF8(message) );
	if (source == kMessageSource_JS) {
		ofLogNotice( " (" + ToUTF8(source_id) + " @ line " + ofToString(line_number) + ", col " + ofToString(column_number) + ")" );
	}
	std::cout << std::endl;

}

//--------------------------------------------------------------
void Manager::update() {
    if (renderer) {
        renderer->Update();
        
        for (auto& it : assets) {
            it.second->update();
        }
        
        renderer->Render();
    }
}

//--------------------------------------------------------------
void Manager::draw() {
    for (auto& it : assets) {
        if (it.second->DOMready) {
            it.second->draw();
        }
    }
}

void Manager::draw(int i) {
    assets[i]->draw();
}

void Manager::removeView(int _id) {
    delete assets[_id];
    assets.erase(_id);
}

//--------------------------------------------------------------
void Manager::keyPressed(int key) {
    for (auto& it : assets) {
        it.second->keyPressed(key);
    }
}

//--------------------------------------------------------------
void Manager::keyReleased(int key) {

}

//--------------------------------------------------------------
void Manager::mouseMoved(int x, int y) {
    for (auto& it : assets) {
        it.second->mouseMoved(x, y);
    }
}

//--------------------------------------------------------------
void Manager::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void Manager::mousePressed(int x, int y, int button) {
    for (auto& it : assets) {
        it.second->mousePressed(x, y, button);
    }
}

//--------------------------------------------------------------
void Manager::mouseReleased(int x, int y, int button) {
    for (auto& it : assets) {
        it.second->mouseReleased(x, y, button);
    }
}

//--------------------------------------------------------------
void Manager::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void Manager::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void Manager::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    for (auto& it : assets) {
        it.second->mouseScrolled(x, y, scrollX, scrollY);
    }
}

//--------------------------------------------------------------
void Manager::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void Manager::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void Manager::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
string Manager::getStringFromJSstr(JSString str) {
	auto length = JSStringGetLength(str);
	auto buffer = new char[length];
	JSStringGetUTF8CString(str, buffer, length);
	
	return (string)buffer;
}

namespace ofxUltralight {

Manager* get() {
    return Manager::get();
}

bool setup() {
    return Manager::get()->setup();
}

void update() {
    Manager::get()->update();
}

ViewAsset* createView(int width, int height, string url) {
    return Manager::get()->createView(width, height, url);
}

ViewAsset* createView(int width, int height, ofVec2f t_offset, string url) {
    return Manager::get()->createView(width, height, t_offset, url);
}

void removeView(int _id) {
    Manager::get()->removeView(_id);
}

void draw(int _id) {
    Manager::get()->draw(_id);
}

}
