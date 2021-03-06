/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if defined(CHROMIUM)
#pragma once

#include "chromium/ChromiumApp.hpp"
#include "chromium/ChromiumRenderHandler.hpp"
#include "chromium/ChromiumRenderProcessHandler.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOM.hpp"
#include "include/cef_app.h"
#include "include/cef_render_handler.h"
#include "include/cef_client.h"

namespace chromium
{
	class ChromiumPimpl
	{
	public:
		dom::ChromiumDOMEngine::Ptr domEngine;
		dom::ChromiumDOM::Ptr mainDOM;

		CefMainArgs* mainArgs;

		CefRefPtr<CefBrowser> browser;
		CefRefPtr<ChromiumApp> app;

		CefRefPtr<ChromiumRenderHandler> renderHandler;
		CefRefPtr<ChromiumRenderProcessHandler> renderProcessHandler;

		CefRefPtr<CefV8Context> v8Context;
	};
}
#endif