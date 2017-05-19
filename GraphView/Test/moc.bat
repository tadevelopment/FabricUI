set QtDir=%1
set Dir=%2
set SGDir=%3

echo // MOC > %Dir%/Moc.cpp
%QtDir%/bin/moc %SGDir%/Native/FabricUI/GraphView/Test/Main.h >> %Dir%/Moc.cpp
%QtDir%/bin/moc %SGDir%/Native/FabricUI/DFG/DFGTabSearchWidget.h >> %Dir%/Moc.cpp
for %%f in (%SGDir%\Native\FabricUI\DFG\TabSearch\*.h) do ( %QtDir%/bin/moc %%f >> %Dir%/Moc.cpp )
%QtDir%/bin/moc %SGDir%/Native/FabricUI/Style/FabricStyle.h >> %Dir%/Moc.cpp
for %%f in (%SGDir%\Native\FabricUI\GraphView\*.h) do ( %QtDir%/bin/moc %%f >> %Dir%/Moc.cpp )
