@ECHO OFF
ECHO ========================
ECHO Launching Alembic Viewer
ECHO ========================
ECHO.
REM ==================================
REM Calls the Fabric Environment .bat
REM ==================================

CALL "%cd%\..\..\..\environment.bat"

REM ==================================
REM Adds the parent directory to the
REM PYTHONPATH environment variable.
REM ==================================

set "PYTHONPATH=%PYTHONPATH%;%cd%\.."

REM ==================================
REM Launches the Python application.
REM ==================================

call python alembic_viewer.py
