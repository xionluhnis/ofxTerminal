#pragma once

#include "ofConstants.h"
#include "ofTexture.h"
#include "ofGraphics.h"
#include "ofTypes.h"
#include "ofUtils.h"
#include "ofEvents.h"

#include <list>

class ofxTerminal : public ofBaseDraws {
public:
    
    // command event notify(input, output)
    ofEvent<const string &> commandSent;

    /*
    enum Orientation {
        TopDown,
        BottomUp
    } orientation;
     */
    ofColor bgColor;
    ofColor fgColor;
    ofColor inColor;
    int margin;
    float lineRatio;

    ofxTerminal(const string &welcomeMessage = "ofxTerminal.") {
        focus = nextFocus = true;
        //orientation = TopDown;
        bgColor = ofColor(0, 0, 0, 100);
        fgColor = ofColor(200, 200, 200);
        inColor = ofColor(255, 255, 255);
        width = 200;
        height = 200;
        margin = 20;
        lineRatio = 1.5f;
        lines.push_back(Line());
        output() += welcomeMessage;
        ofAddListener(ofEvents().keyPressed, this, &ofxTerminal::keyPressed);
        ofAddListener(ofEvents().keyReleased, this, &ofxTerminal::keyReleased);
    }

    virtual void draw(float x, float y) const {
        draw(x, y, width, height);
    }

    virtual void draw(float x, float y, float w, float h) const {
        ofPushMatrix();
        ofTranslate(x, y);
        
        // 1 = background
        ofFill();
        ofSetColor(bgColor);
        ofDrawRectangle(0, 0, w, h);
        
        // 2 = text, from the end
        y = h - margin;
        x = margin;
        ofRectangle box = font.getBoundingBox("> ", 0, 0);
        int lineHeight = box.height * lineRatio;
        int dx = box.width * 1.1f;
        for(auto lineIt = lines.rbegin(); lineIt != lines.rend() && y >= margin; ++lineIt){
            const Line &line = *lineIt;
            
            // input line
            ofSetColor(inColor);
            ofDrawBitmapString(">", x, y);
            ofDrawBitmapString(line.input, x + dx, y); // TODO fix text overflow!
            y -= lineHeight;
            
            // output lines
            if(!line.output.empty()){
                vector<string> subLines = line.splitOutput();
                ofSetColor(fgColor);
                for(auto it = subLines.rbegin(); it != subLines.rend() && y >= margin; ++it) {
                    const string &subLine = *it;
                    ofDrawBitmapString(subLine, x, y); // TODO fix text overflow
                    y -= lineHeight;
                }
            }
            if(!line.outputLines.empty()) {
                ofSetColor(fgColor);
                for(auto it = line.outputLines.rbegin(); it != line.outputLines.rend() && y >= margin; ++it) {
                    const string &subLine = *it;
                    ofDrawBitmapString(subLine, x, y); // TODO fix text overflow
                    y -= lineHeight;
                }
            }
        }
        
        ofPopMatrix();
    }

    void setHeight(int value) {
        if (value > 100) {
            height = value;
        }
    }

    virtual float getHeight() const {
        return height;
    }

    void setWidth(int value) {
        if (value > 100) {
            width = value;
        }
    }

    virtual float getWidth() const {
        return width;
    }

    void keyPressed(ofKeyEventArgs &event) {
        ofLog(OF_LOG_VERBOSE, "keyPressed: #%d=%c, focus=%s", event.key, event.key, focus ? "true" : "false");
        if (!focus) {
            return;
        }
        switch (event.key) {

            case OF_KEY_RETURN: {
                const string &command = input();
                newLine(); // so that new output go to the right place
                ofNotifyEvent(commandSent, command);
            } break;

            case OF_KEY_BACKSPACE:
                // TODO use carret when it will be implemented
                if(!input().empty()){
                    input().pop_back();
                }
                break;
                
            case OF_KEY_DEL:
                // TODO have a moveable carret => delete will delete current character
                break;

            case OF_KEY_TAB:
                setFocus(false, true);
                break;

            default:
                if(event.key >= ' ' && event.key <= 255){
                    // printable character
                    input() += ofToString(char(event.key));
                }
                break;
        }
    }

    void keyReleased(ofKeyEventArgs &args) {
        focus = nextFocus;
    }

    const string &input() const {
        return lines.back().input;
    }
    
    string &output() {
        return lines.back().output;
    }
    
    void setFocus(bool f, bool delayed = false) {
        if(delayed){
            nextFocus = f;
        } else {
            focus = nextFocus = f;
        }
    }
    bool hasFocus() const {
        return focus;
    }

protected:
    
    void newLine() {
        // reduce output to multiple sublines
        lines.back().mergeOutput();
        // create new line
        lines.push_back(Line());
    }

    string &input() {
        return lines.back().input;
    }

private:
    ofBitmapFont font;
    bool focus, nextFocus;
    int width;
    int height;

    struct Line {
        string input;
        string output;
        std::list<string> outputLines;

        Line() : input(""), output("") {
        }
        
        vector<string> splitOutput() const {
            return ofSplitString(output, "\n", true);
        }
        
        void mergeOutput() {
            vector<string> newLines = splitOutput();
            for(unsigned int i = 0; i < newLines.size(); ++i){
                outputLines.push_back(newLines[i]);
            }
            output.clear(); // remove unsplit output
        }
    };
    std::list<Line> lines;
};
