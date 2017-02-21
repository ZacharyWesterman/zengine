#pragma once
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <irrlicht.h>

#include "eventHandler.h"

#include "../core/string.h"
#include "../core/array.h"
#include "../script/scriptParser.h"
#include "../file/loadFileToMemory.h"
#include "../file/saveToFileFromMemory.h"

#include "global/fps.h"
#include "global/script_delay_timer.h"

#include "../core/stringUtils/convert_type.h"
#include "pure_string.h"

#include "deviceSettings.h"

#include "../core/pause.h"

#include <iostream>

/*#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif*/



void setSkinTransparency(irr::s32 alpha, irr::gui::IGUISkin* skin)
{
    for (irr::s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
    {
        irr::video::SColor col = skin->getColor((irr::gui::EGUI_DEFAULT_COLOR)i);
        col.setAlpha(alpha);
        skin->setColor((irr::gui::EGUI_DEFAULT_COLOR)i, col);
    }
}

irr::video::SColor getScriptColor(const core::string<wchar_t>& text)
{
    irr::u32 color = (irr::u32)core::value(text);

    irr::u32 red   = ((long)color & 0xFF000000) >> 24;
    irr::u32 green = ((long)color & 0x00FF0000) >> 16;
    irr::u32 blue  = ((long)color & 0x0000FF00) >> 8;
    irr::u32 alpha = ((long)color & 0x000000FF);

    return irr::video::SColor(alpha, red, green, blue);
}


namespace engine
{
    class game
    {
        enum GUI_ELEMENTS
        {
            GUI_SCRIPT_WINDOW       = -2,

            GUI_SCRIPT_ERROR_BOX    = -3,
            GUI_SCRIPT_EDIT_BOX     = -4,

            GUI_SCRIPT_PARSE_BTN    = -5,
            GUI_SCRIPT_SAVE_BTN     = -6,
            GUI_SCRIPT_STOP_BTN     = -7,

            GUI_EXIT_BUTTON         = -8,
            GUI_SCRIPT_BUTTON       = -9,
            GUI_SETTINGS_BUTTON     = -10,
            GUI_CONSOLE_BUTTON      = -11,

            GUI_SETTINGS_WINDOW     = -12,

            GUI_CONSOLE_WINDOW      = -13,
            GUI_CONSOLE_LIST        = -14,
        };

    private:
        deviceSettings settings;

        double Max_FPS;


        irr::IrrlichtDevice *device;

        irr::video::IVideoDriver *driver;
        irr::scene::ISceneManager *scene;
        irr::gui::IGUIEnvironment *gui;


        eventHandler *event;


        core::string<wchar_t> script;
        script::scriptParser<wchar_t> parser;
        bool script_has_error;


        irr::video::SColor sceneColor;

        void init_script_window();
        void init_button_menu();
        void init_settings_window();
        void init_console();


        void handle_script_window();
        void handle_button_menu();
        void handle_settings_window();

        void check_script_contextual_errors();
        void notify_of_error(const script::command<wchar_t>&);

        void print_to_console(const core::string<wchar_t>&);

        core::string<wchar_t> get_error_string(script::error_flag);


    public:
        game();
        ~game();

        bool run();
    };


    //initialize the game engine
    game::game()
    {
        settings.load("SETTINGS.cfg");

        Max_FPS = 60.0;



        script_has_error = false;

        //if script file exists, load it
        if (!file::loadFileToMemory("GAME.script", script))
        {
            //otherwise load a sample script
            //script = L"#basic hello world script\nmain\nprint \"Hello world!\"\nendmain";
        }

        //parse the script before the window launches
        parser.parse(script);


        device = irr::createDevice(irr::video::EDT_OPENGL, //rendering engine
                                   irr::core::dimension2d<irr::u32>(settings.width, settings.height), //window dimensions
                                   settings.bits, //32-bit colors
                                   settings.fullscreen, //fullscreen?
                                   settings.shadows, //stencil buffer? (for shadows)
                                   settings.vsync, //vsync enabled? (only useful in fullscreen)
                                   0); //event receiver


        if (device)
        {
            driver = device->getVideoDriver();
            scene = device->getSceneManager();
            gui = device->getGUIEnvironment();

            event = new eventHandler(device);
            device->setEventReceiver(event);


            device->setWindowCaption(L"ZEngine");


            setSkinTransparency(255, gui->getSkin());

            //by default the scene color is:
            sceneColor = irr::video::SColor(255,100,101,140);

            //init_console();

            if (settings.Mode == mode::DEBUG)
            {
                //init_script_window();
                init_button_menu();
                //init_settings_window();
            }


            //set the default font
            irr::gui::IGUIFont* font = gui->getFont("fonts/DEFAULT.xml");
            if (font)
            {
                gui->getSkin()->setFont(font);
            }
            else
            {
                gui->getSkin()->setFont(gui->getBuiltInFont());
            }
        }
    }



    game::~game()
    {
        if (device)
        {
            device->closeDevice();

            delete event;
        }
    }



    void game::init_script_window()
    {
        //create the text window
        irr::gui::IGUIWindow *script_window =
                     gui->addWindow(irr::core::rect<irr::s32>(60,30,460,330),
                                    false, //modal?
                                    L"Script",//title
                                    0,//parent
                                    GUI_SCRIPT_WINDOW);//id


        int start_x = script_window->getClientRect().UpperLeftCorner.X;
        int start_y = script_window->getClientRect().UpperLeftCorner.Y;

        int width_max = script_window->getClientRect().getWidth();
        int height_max = script_window->getClientRect().getHeight();

        int btn_width = 50;
        int btn_height = 20;

        int err_height = 20;

        int sep = 5;

        //the text window consists mainly of an edit box
        irr::gui::IGUIEditBox *script_edit_box =
                       gui->addEditBox(script.str(),
                                       irr::core::rect<irr::s32>(start_x,
                                                                 start_y,
                                                                 start_x+width_max-btn_width-sep*2,
                                                                 start_y+height_max-err_height-sep),
                                       true, //border?
                                       script_window,//parent
                                       GUI_SCRIPT_EDIT_BOX);//id

        //some aesthetic details for the edit box
        script_edit_box->setMultiLine(true);
        //script_edit_box->setDrawBackground(false);
        //script_edit_box->setOverrideColor(irr::video::SColor(255, 255, 255, 255));



        //irr::gui::IGUIButton *script_save_btn =
                        gui->addButton(irr::core::rect<irr::s32>(start_x+width_max-btn_width-sep,
                                                                 start_y+sep,
                                                                 start_x+width_max-sep,
                                                                 start_y+sep+btn_height),
                                       script_window,
                                       GUI_SCRIPT_SAVE_BTN,
                                       L"Save",
                                       L"Save the current script.");


        //irr::gui::IGUIButton *script_parse_btn =
                        gui->addButton(irr::core::rect<irr::s32>(start_x+width_max-btn_width-sep,
                                                                 start_y+btn_height+sep*2,
                                                                 start_x+width_max-sep,
                                                                 start_y+btn_height*2+sep*2),
                                       script_window,
                                       GUI_SCRIPT_PARSE_BTN,
                                       L"Run",
                                       L"Parse and run the script.");


        //irr::gui::IGUIButton *script_stop_btn =
                        gui->addButton(irr::core::rect<irr::s32>(start_x+width_max-btn_width-sep,
                                                                 start_y+btn_height*2+sep*3,
                                                                 start_x+width_max-sep,
                                                                 start_y+btn_height*3+sep*3),
                                       script_window,
                                       GUI_SCRIPT_STOP_BTN,
                                       L"Stop",
                                       L"Stop running the script.");


        //there is also a text box to show errors
        irr::gui::IGUIStaticText *script_error_box =
                       gui->addStaticText(L"Ready.",
                                       irr::core::rect<irr::s32>(start_x,
                                                                 start_y+height_max-err_height,
                                                                 start_x+width_max-btn_width-sep*2,
                                                                 start_y+height_max),
                                       false, //border?
                                       false, //word-wrap?
                                       script_window,//parent
                                       GUI_SCRIPT_ERROR_BOX,//id
                                       true);//fill background?

        script_error_box->setOverrideColor(irr::video::SColor(255, 160, 0, 0));

    }


    void game::init_button_menu()
    {
        //irr::gui::IGUIButton *exit_btn =
                        gui->addButton(irr::core::rect<irr::s32>(0,0,60,20),
                                       0,
                                       GUI_EXIT_BUTTON,
                                       L"Exit",
                                       L"Exit the application.");

        //irr::gui::IGUIButton *script_btn =
                        gui->addButton(irr::core::rect<irr::s32>(0,25,60,45),
                                       0,
                                       GUI_SCRIPT_BUTTON,
                                       L"Script",
                                       L"Show the current script.");

        //irr::gui::IGUIButton *script_btn =
                        gui->addButton(irr::core::rect<irr::s32>(0,50,60,70),
                                       0,
                                       GUI_SETTINGS_BUTTON,
                                       L"Settings",
                                       L"Show application settings.");

        //irr::gui::IGUIButton *console_btn =
                        gui->addButton(irr::core::rect<irr::s32>(0,75,60,95),
                                       0,
                                       GUI_CONSOLE_BUTTON,
                                       L"Console",
                                       L"Show the console.");
    }


    void game::init_settings_window()
    {
        //irr::gui::IGUIWindow *settings_window =
                     gui->addWindow(irr::core::rect<irr::s32>(100,70,430,330),
                                    false, //modal?
                                    L"Settings",//title
                                    0,//parent
                                    GUI_SETTINGS_WINDOW);//id
    }


    void game::init_console()
    {
        //create the console window
        irr::gui::IGUIWindow *console_window =
                     gui->addWindow(irr::core::rect<irr::s32>(60,30,460,330),
                                    false, //modal?
                                    L"Console",//title
                                    0,//parent
                                    GUI_CONSOLE_WINDOW);//id


        int min_x = console_window->getClientRect().UpperLeftCorner.X;
        int min_y = console_window->getClientRect().UpperLeftCorner.Y;
        int max_x = console_window->getClientRect().getWidth() + min_x;
        int max_y = console_window->getClientRect().getHeight() + min_y;

        //irr::gui::IGUIListBox* console =
            gui->addListBox(irr::core::rect<irr::s32>(min_x,min_y,max_x,max_y),
                            console_window, //parent
                            GUI_CONSOLE_LIST, //id
                            false);//draw background?*/
    }


    bool game::run()
    {

        if (device && device->run())
        {
            //set the global fps so the script function 'fps()' stays accurate
            global::frame.reset();


            ///Draw the scene
            driver->beginScene(true, false, sceneColor);

            scene->drawAll();
            gui->drawAll();

            driver->endScene();


            ///deal with the script
            ///

            if (!script_has_error)
                check_script_contextual_errors();

            if (!script_has_error && !parser.done() &&
                (global::SCRIPT_DELAY_TIMER.millis() >= global::SCRIPT_DELAY_TIME))
            {
                script::command<wchar_t> cmd;

                if (parser.next(cmd))
                {
                    if (cmd.error)
                    {
                        notify_of_error(cmd);
                    }
                    else
                    {
                        if (cmd.type == script::cmd::PRINT_TO_CONSOLE)
                        {
                            print_to_console(cmd.data[0]);
                        }
                        else if (cmd.type == script::cmd::SET_WINDOW_TITLE)
                        {
                            device->setWindowCaption(pure_string(cmd.data[0]).str());
                        }
                        else if (cmd.type == script::cmd::EXIT_PROGRAM)
                        {
                            return false;
                        }
                        else if (cmd.type == script::cmd::SET_SCENE_COLOR)
                        {
                            sceneColor = getScriptColor(cmd.data[0]);
                        }
                        else if (cmd.type == script::cmd::DELAY_SCRIPT)
                        {
                            global::SCRIPT_DELAY_TIME = core::value(cmd.data[0]);

                            global::SCRIPT_DELAY_TIMER.reset();
                        }
                        else if (cmd.type == script::cmd::SET_MAX_FPS)
                        {
                            Max_FPS = core::value(cmd.data[0]);
                        }
                    }
                }
            }


            if (settings.Mode == mode::DEBUG)
            {
                handle_button_menu();

                handle_script_window();
                handle_settings_window();
            }


            event->reset();


            double frame_seconds = global::frame.micros()/1000000;

            if ((Max_FPS > 0.0) &&
                (frame_seconds > 0) &&
                (frame_seconds < (1.0/Max_FPS)))
            {
                core::pause(((1.0/Max_FPS) - frame_seconds)*1000);
            }


            if (global::frame.micros() <= 0.0)
            {
                global::FPS = (double)-1;
            }
            else
            {
                global::FPS = 1000000 / global::frame.micros();
            }


            return true;
        }
        else
        {
            return false;
        }
    }


    void game::handle_script_window()
    {
        if (event->buttonPress())
        {
            irr::s32 id = event->whatId();

            if (id == GUI_SCRIPT_SAVE_BTN)
            {
                //get the edit box's text
                irr::gui::IGUIEditBox *edit_box =
                    (irr::gui::IGUIEditBox*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SCRIPT_EDIT_BOX, true);

                if (edit_box)
                {
                    script = edit_box->getText();
                    //and save that text to a file
                    file::saveStringToFile("GAME.script",
                                            script);
                }
            }
            else if (id == GUI_SCRIPT_PARSE_BTN)
            {
                //get the edit box's text
                irr::gui::IGUIEditBox *edit_box =
                    (irr::gui::IGUIEditBox*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SCRIPT_EDIT_BOX, true);

                if (edit_box)
                {
                    parser.stop();
                    script_has_error = false;

                    script = edit_box->getText();
                    //and parse that text
                    parser.parse(script);
                }
            }
            else if (id == GUI_SCRIPT_STOP_BTN)
            {
                //stop the parser
                parser.stop();
            }
        }
    }


    void game::handle_button_menu()
    {
        if (event->buttonPress())
        {
            irr::s32 id = event->whatId();


            if (id == GUI_EXIT_BUTTON)
            {
                device->drop();
                device = 0;
            }
            else if (id == GUI_SCRIPT_BUTTON)
            {
                irr::gui::IGUIWindow *script_window =
                    (irr::gui::IGUIWindow*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SCRIPT_WINDOW, true);

                if (!script_window)
                    init_script_window();

                if (script_window)
                    gui->getRootGUIElement()->bringToFront(script_window);
            }
            else if (id == GUI_SETTINGS_BUTTON)
            {
                irr::gui::IGUIWindow *settings_window =
                    (irr::gui::IGUIWindow*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SETTINGS_WINDOW, true);

                if (!settings_window)
                    init_settings_window();

                if (settings_window)
                    gui->getRootGUIElement()->bringToFront(settings_window);
            }
            else if (id == GUI_CONSOLE_BUTTON)
            {
                irr::gui::IGUIWindow *console_window =
                    (irr::gui::IGUIWindow*)gui->getRootGUIElement()->
                    getElementFromId(GUI_CONSOLE_WINDOW, true);

                if (!console_window)
                    init_console();

                if (console_window)
                    gui->getRootGUIElement()->bringToFront(console_window);
            }
        }
    }


    void game::handle_settings_window()
    {
        if (event->buttonPress())
        {
            //irr::s32 id = event->whatId();


        }
    }



    void game::check_script_contextual_errors()
    {
        script::command<wchar_t> cmd;

        if (parser.get_next_error(cmd))
        {
            notify_of_error(cmd);
        }
        else
        {
            //get the error box
            irr::gui::IGUIEditBox *error_box =
                (irr::gui::IGUIEditBox*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SCRIPT_ERROR_BOX, true);

            if (error_box)
            {
                //and empty its text
                error_box->setText(L"No errors.");

            }
        }
    }


    void game::notify_of_error(const script::command<wchar_t>& cmd)
    {
        if (cmd.error)
        {
            script_has_error = true;

            //get the error box
            irr::gui::IGUIEditBox *error_box =
                (irr::gui::IGUIEditBox*)gui->getRootGUIElement()->
                    getElementFromId(GUI_SCRIPT_ERROR_BOX, true);

            if (error_box)
            {
                //and set its text to a description of the error
                core::string<wchar_t> error_string = L"Error [";
                error_string += script::cmd::WCHAR_T_COMMANDS[cmd.type];


                for (int i=0; i<cmd.data.size(); i++)
                {
                    error_string += L", ";
                    error_string += cmd.data[i];
                }

                error_string += L"]: ";
                error_string += get_error_string(cmd.error);

                error_box->setText(error_string.str());

            }
        }
    }


    core::string<wchar_t> game::get_error_string(script::error_flag error)
    {
        core::array< core::string<wchar_t> > err_list;

        if (error)
        {
            if (error & script::error::DIV_BY_ZERO)
                err_list.add(L"DIV_BY_ZERO");

            if (error & script::error::INVALID_CHARACTER)
                err_list.add(L"INVALID_CHARACTER");

            if (error & script::error::MISSING_OPERAND)
                err_list.add(L"MISSING_OPERAND");

            if (error & script::error::INVALID_OPERATION)
                err_list.add(L"INVALID_OPERATION");

            if (error & script::error::MISSING_L_PARENTH)
                err_list.add(L"MISSING_L_PARENTH");

            if (error & script::error::MISSING_R_PARENTH)
                err_list.add(L"MISSING_R_PARENTH");

            if (error & script::error::TOO_FEW_PARAMS)
                err_list.add(L"TOO_FEW_PARAMS");

            if (error & script::error::TOO_MANY_PARAMS)
                err_list.add(L"TOO_MANY_PARAMS");

            if (error & script::error::INVALID_PARAM)
                err_list.add(L"INVALID_PARAM");

            if (error & script::error::NON_REAL_NUMBER)
                err_list.add(L"NON_REAL_NUMBER");

            if (error & script::error::OUT_OF_BOUNDS)
                err_list.add(L"OUT_OF_BOUNDS");

            if (error & script::error::UNKNOWN_VARIABLE)
                err_list.add(L"UNKNOWN_VARIABLE");

            if (error & script::error::WRONG_VAR_TYPE)
                err_list.add(L"WRONG_VAR_TYPE");

            if (error & script::error::BAD_FORMATTING)
                err_list.add(L"BAD_FORMATTING");

            if (error & script::error::MISSING_OPERATOR)
                err_list.add(L"MISSING_OPERATOR");

            if (error & script::error::UNKNOWN_COMMAND)
                err_list.add(L"UNKNOWN_COMMAND");

            if (error & script::error::BAD_REDEFINITION)
                err_list.add(L"BAD_REDEFINITION");

            if (error & script::error::NO_ENTRY_POINT)
                err_list.add(L"NO_ENTRY_POINT");

            if (error & script::error::BAD_SCOPING)
                err_list.add(L"BAD_SCOPING");

            if (error & script::error::UNEXP_COMMAND)
                err_list.add(L"UNEXP_COMMAND");

            if (error & script::error::UNEXP_END_OF_SCRIPT)
                err_list.add(L"UNEXP_END_OF_SCRIPT");

            if (error & script::error::UNKNOWN_LABEL)
                err_list.add(L"UNKNOWN_LABEL");

            if (error & script::error::UNKNOWN_SUBROUTINE)
                err_list.add(L"UNKNOWN_SUBROUTINE");

        }


        core::string<wchar_t> output;

        if (err_list.size() >= 1)
        {
            output = err_list.at(0);

            for (int i=1; i<err_list.size(); i++)
            {
                output += L", ";
                output += err_list.at(i);
            }
        }

        return output;
    }


    void game::print_to_console(const core::string<wchar_t>& scriptText)
    {
        //get the console
        irr::gui::IGUIListBox *console =
            (irr::gui::IGUIListBox*)gui->getRootGUIElement()->
            getElementFromId(GUI_CONSOLE_LIST, true);

        //and add the text to it
        if (console)
        {
            core::string<wchar_t> text = pure_string(scriptText);

            console->addItem(text.str());

            if (console->getItemCount() > 12)
            {
                console->removeItem(0);
            }
        }
    }
}

#endif // GAME_H_INCLUDED