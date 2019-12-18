#pragma once

#define NOMINMAX
#define ASMJIT_EMBED
#include <asmjit.h>

// to enable including additional Windows headers
#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>
#include <ntstatus.h>
#include <psapi.h>
#include <ntstatus.h>

#include <iostream>
#include <stdexcept>
#include <exception>
#include <vector>
#include <sstream> 

// to avoid linkage warnings
#define WOW64EXT_EXPORTS
#include <wow64ext.h>
#include <internal.h>
