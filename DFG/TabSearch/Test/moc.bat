set QtDir=%1
set Dir=%2

echo // MOC > %Dir%/Moc.cpp
%QtDir%/bin/moc %Dir%/../../DFGTabSearchWidget.h >> %Dir%/Moc.cpp
%QtDir%/bin/moc %Dir%/../DFGPresetSearchWidget.h >> %Dir%/Moc.cpp
%QtDir%/bin/moc %Dir%/../QueryEdit.h >> %Dir%/Moc.cpp
%QtDir%/bin/moc %Dir%/../ResultsView.h >> %Dir%/Moc.cpp
