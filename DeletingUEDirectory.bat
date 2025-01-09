
@echo off
chcp 65001
if exist "Binaries" rmdir /s /q "Binaries"
if exist "Intermediate" rmdir /s /q "Intermediate"
if exist "Saved" rmdir /s /q "Saved"
echo Delete operation complete.