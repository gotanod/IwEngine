xcopy /y /f "./IwEngine/bin/Release.windows.x86_64/*.dll" "./_export/bin/Release.windows.x86_64/"
xcopy /y /f "./IwEngine/lib/Release.windows.x86_64/*.lib" "./_export/lib/Release.windows.x86_64/"
xcopy /y /f /e "./IwEngine/include/*" "./_export/include/"
xcopy /y /f /e "./IwEngine/extern/imgui/include/*" "./_export/include/"
xcopy /y /f /e "./IwEngine/extern/fmod/include/*" "./_export/include/"
xcopy /y /f /e "./IwEngine/extern/json/include/*" "./_export/include/"
xcopy /y /f "./IwEngine/extern/imgui/lib/Release.windows.x86_64/ImGui.lib" "./_export/lib/Release.windows.x86_64"
xcopy /y /f /e "./IwEngine/extern/glm/include/*" "./_export/include/"
xcopy /y /f /e "./IwEngine/extern/asio/include/*" "./_export/include/"