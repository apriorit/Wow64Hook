#pragma once

// to enable including additional Windows headers
#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ntstatus.h>

// to avoid linkage warnings
#define WOW64EXT_EXPORTS
#include <wow64ext.h>
#include <internal.h>
