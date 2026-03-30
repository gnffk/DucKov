// 명령어				옵션			원본파일이 있는 위치			사본파일을 저장할 위치

xcopy				/y			.\Engine\public\*.h			.\EngineSDK\Inc\
xcopy				/y			.\Engine\Bin\Engine.dll		.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\

xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imgui.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imconfig.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imgui_internal.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imstb_rectpack.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imstb_textedit.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\imgui-1.83\imstb_truetype.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\ImGuizmo-1.83\ImGuizmo.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\ImGuizmo-1.83\ImSequencer.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\ImGuizmo-1.83\ImZoomSlider.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\ImGuizmo-1.83\ImCurveEdit.h		.\EngineSDK\Inc\
xcopy				/y/I			.\Engine\lib\imgui\ImGuizmo-1.83\GraphEditor.h		.\EngineSDK\Inc\


xcopy				/y/I			.\Engine\lib\assimp\assimp-vc143-mtd.dll 		.\Client\Bin\