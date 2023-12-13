#include "Graphics.h"


// Define pointers to the GL functions we need that aren't in opengl32.lib.

#undef GLEXTFUNC
#define GLEXTFUNC(type, name, params) type (*name)params = nullptr;
#include "OpenGL.h"

// This macro is used to retrieve function pointers from the GL driver.

#define GLGETPROC(name) *reinterpret_cast<PROC *>(&name) = wglGetProcAddress(#name)


using namespace Engine;


namespace
{
	enum
	{
		kShadowMapSize = 1024
	};
}


const char Program::prologString[] =
{
	"#version 450\n"

	"#define float2 vec2\n"
	"#define float3 vec3\n"
	"#define float4 vec4\n"
	"#define int2 ivec2\n"
	"#define int3 ivec3\n"
	"#define int4 ivec4\n"
	"#define uint2 uvec2\n"
	"#define uint3 uvec3\n"
	"#define uint4 uvec4\n"

	"#define ddx dFdx\n"
	"#define ddy dFdy\n"
	"#define lerp mix\n"
	"#define frac fract\n"
	"#define rsqrt inversesqrt\n"
	"#define asint floatBitsToInt\n"
	"#define asuint floatBitsToUint\n"
	"#define asfloat uintBitsToFloat\n"
	"#define saturate(x) clamp(x, 0.0, 1.0)\n"

	"#define fragment_position gl_FragCoord\n"
	"#define fragment_w (1.0 / gl_FragCoord.w)\n"
	"#define front_facing gl_FrontFacing\n"
	"#define facing_sign (float(gl_FrontFacing) * 2.0 - 1.0)\n"
};


GraphicsManager *Engine::graphicsManager = nullptr;


Engine::Buffer::Buffer(uint32 size, const void *data)
{
	glCreateBuffers(1, &bufferObject);
	glNamedBufferData(bufferObject, size, data, GL_STATIC_DRAW);
}

Engine::Buffer::~Buffer()
{
	glDeleteBuffers(1, &bufferObject);
}

volatile void *Engine::Buffer::MapBuffer(void)
{
	return (glMapNamedBuffer(bufferObject, GL_WRITE_ONLY));
}

void Engine::Buffer::UnmapBuffer(void)
{
	glUnmapNamedBuffer(bufferObject);
}

void Engine::Buffer::BindVertexBuffer(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
}

void Engine::Buffer::BindIndexBuffer(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject);
}

void Engine::Buffer::BindUniformBuffer(void)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferObject);
}


VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &vertexArrayObject);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vertexArrayObject);
}

void VertexArray::SetAttribBuffer(int32 binding, int32 stride, const Buffer *buffer)
{
	glVertexArrayVertexBuffer(vertexArrayObject, binding, buffer->bufferObject, 0, stride);
}

void VertexArray::SetAttribArray(int32 index, int32 count, int32 format, int32 offset, int32 binding)
{
	static const GLenum formatTable[kFormatCount] =
	{
		GL_FLOAT, GL_UNSIGNED_BYTE
	};

	glEnableVertexArrayAttrib(vertexArrayObject, index);
	glVertexArrayAttribFormat(vertexArrayObject, index, count, formatTable[format], GL_TRUE, offset);
	glVertexArrayAttribBinding(vertexArrayObject, index, binding);
}

void VertexArray::BindVertexArray()
{
	glBindVertexArray(vertexArrayObject);
}


Program::Program(int32 vertexStringCount, const char **vertexStringArray, int32 fragmentStringCount, const char **fragmentStringArray)
{
	GLint	status, size;

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, vertexStringCount, vertexStringArray, nullptr);
	glCompileShader(vertexShaderObject);

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &size);
		char *string = new char[size];
		glGetShaderInfoLog(vertexShaderObject, size, nullptr, string);
		OutputDebugStringA("Vertex shader error:\n");
		OutputDebugStringA("(Subtract 23 from line number)\n");
		OutputDebugStringA(string);
		delete[] string;
	}

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, fragmentStringCount, fragmentStringArray, nullptr);
	glCompileShader(fragmentShaderObject);

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &size);
		char *string = new char[size];
		glGetShaderInfoLog(fragmentShaderObject, size, nullptr, string);
		OutputDebugStringA("Fragment shader error:\n");
		OutputDebugStringA("(Subtract 23 from line number)\n");
		OutputDebugStringA(string);
		delete[] string;
	}

	programObject = glCreateProgram();
	glAttachShader(programObject, vertexShaderObject);
	glAttachShader(programObject, fragmentShaderObject);
	glLinkProgram(programObject);

	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &size);
		char *string = new char[size];
		glGetProgramInfoLog(programObject, size, nullptr, string);
		OutputDebugStringA("Program link error:\n");
		OutputDebugStringA("(Subtract 23 from line number)\n");
		OutputDebugStringA(string);
		delete[] string;
	}
}

Program::Program(const char *vertexString, const char *fragmentString) : Program(2, StringPair(prologString, vertexString), 2, StringPair(prologString, fragmentString))
{
}

Program::~Program()
{
	glDeleteProgram(programObject);
	glDeleteShader(fragmentShaderObject);
	glDeleteShader(vertexShaderObject);
}

void Program::BindProgram(void)
{
	glUseProgram(programObject);
}


Texture::Texture(int32 type, int32 format, int32 width, int32 height, int32 depth, int32 mipmapCount, const void *image)
{
	static const GLenum targetTable[kTypeCount] =
	{
		GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_2D_MULTISAMPLE
	};

	static const GLenum internalFormatTable[kFormatCount] =
	{
		GL_SRGB8_ALPHA8, GL_RGBA8, GL_RGBA8_SNORM, GL_RG8, GL_RG8_SNORM, GL_R8, GL_R8_SNORM, GL_RGBA16F, GL_RG16UI, GL_DEPTH_COMPONENT32F
	};

	static const GLenum formatTable[kFormatCount] =
	{
		GL_RGBA, GL_RGBA, GL_RGBA, GL_RG, GL_RG, GL_RED, GL_RED, GL_RGBA, GL_RG_INTEGER, GL_DEPTH_COMPONENT
	};

	static const GLenum typeTable[kFormatCount] =
	{
		GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, GL_HALF_FLOAT, GL_UNSIGNED_SHORT, GL_FLOAT
	};

	static const int8 sizeTable[kFormatCount] =
	{
		4, 4, 4, 2, 2, 1, 1, 8, 4, 4
	};

	glCreateTextures(targetTable[type], 1, &textureObject);
	glTextureParameteri(textureObject, GL_TEXTURE_MAX_LEVEL, mipmapCount - 1);

	if (format != kFormatUint16Rg)
	{
		glTextureParameteri(textureObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(textureObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameterf(textureObject, GL_TEXTURE_MAX_ANISOTROPY, 4.0F);

		if (format == kFormatDepth)
		{
			glTextureParameteri(textureObject, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			glTextureParameteri(textureObject, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		}
	}
	else
	{
		glTextureParameteri(textureObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(textureObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	const char *data = static_cast<const char *>(image);

	switch (type)
	{
		case kType2D:

			glTextureStorage2D(textureObject, mipmapCount, internalFormatTable[format], width, height);

			for (int32 mipmap = 0; mipmap < mipmapCount; mipmap++)
			{
				glTextureSubImage2D(textureObject, mipmap, 0, 0, width, height, formatTable[format], typeTable[format], data);
				data += width * height * sizeTable[format];
				width = Max(width >> 1, 1);
				height = Max(height >> 1, 1);
			}

			break;

		case kType3D:

			glTextureStorage3D(textureObject, mipmapCount, internalFormatTable[format], width, height, depth);

			for (int32 mipmap = 0; mipmap < mipmapCount; mipmap++)
			{
				glTextureSubImage3D(textureObject, mipmap, 0, 0, 0, width, height, depth, formatTable[format], typeTable[format], data);
				data += width * height * depth * sizeTable[format];
				width = Max(width >> 1, 1);
				height = Max(height >> 1, 1);
				depth = Max(depth >> 1, 1);
			}

			break;

		case kTypeCube:

			glTextureStorage2D(textureObject, mipmapCount, internalFormatTable[format], width, width);

			if (data)
			{
				for (int32 mipmap = 0; mipmap < mipmapCount; mipmap++)
				{
					for (int32 face = 0; face < 6; face++)
					{
						glTextureSubImage3D(textureObject, mipmap, 0, 0, face, width, width, 1, formatTable[format], typeTable[format], data);
						data += width * width * sizeTable[format];
					}

					width = Max(width >> 1, 1);
				}
			}

			glTextureParameteri(textureObject, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(textureObject, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;

		case kType2DArray:

			glTextureStorage3D(textureObject, mipmapCount, internalFormatTable[format], width, height, depth);

			for (int32 mipmap = 0; mipmap < mipmapCount; mipmap++)
			{
				glTextureSubImage3D(textureObject, mipmap, 0, 0, 0, width, height, depth, formatTable[format], typeTable[format], data);
				data += width * height * depth * sizeTable[format];
				width = Max(width >> 1, 1);
				height = Max(height >> 1, 1);
			}

			break;

		case kTypeRectangle:

			glTextureStorage2D(textureObject, 1, internalFormatTable[format], width, height);

			if (data)
			{
				glTextureSubImage2D(textureObject, 0, 0, 0, width, height, formatTable[format], typeTable[format], data);
			}

			break;

		case kTypeMultisample:

			glTextureStorage2DMultisample(textureObject, 4, internalFormatTable[format], width, height, GL_FALSE);
			break;
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureObject);
}

void Texture::BindTexture(int32 unit)
{
	glBindTextures(unit, 1, &textureObject);
}


Font::Font(const char *name) : fontFile(name)
{
	fontHeader = Slug::GetFontHeader(fontFile.GetData());

	int32 curveTextureWidth = fontHeader->curveTextureSize.x;
	int32 curveTextureHeight = fontHeader->curveTextureSize.y;
	Slug::Texel16 *curveTextureData = new Slug::Texel16[curveTextureWidth * curveTextureHeight];

	int32 bandTextureWidth = fontHeader->bandTextureSize.x;
	int32 bandTextureHeight = fontHeader->bandTextureSize.y;
	Slug::BandTexel *bandTextureData = new Slug::BandTexel[bandTextureWidth * bandTextureHeight];

	Slug::ExtractFontTextures(fontHeader, curveTextureData, bandTextureData);
	curveTexture = new Texture(Texture::kType2D, Texture::kFormatFloat16Rgba, curveTextureWidth, curveTextureHeight, 1, 1, curveTextureData);
	bandTexture = new Texture(Texture::kType2D, Texture::kFormatUint16Rg, bandTextureWidth, bandTextureHeight, 1, 1, bandTextureData);

	delete[] bandTextureData;
	delete[] curveTextureData;
}

Font::~Font()
{
	bandTexture->Release();
	curveTexture->Release();
}


Framebuffer::Framebuffer(const Texture *colorTexture, const Texture *depthTexture)
{
	glCreateFramebuffers(1, &framebufferObject);

	if (colorTexture)
	{
		static const GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};

		glNamedFramebufferTexture(framebufferObject, GL_COLOR_ATTACHMENT0, colorTexture->textureObject, 0);
		glNamedFramebufferDrawBuffers(framebufferObject, 1, drawBuffers);
	}
	else
	{
		glNamedFramebufferDrawBuffers(framebufferObject, 0, nullptr);
	}

	if (depthTexture)
	{
		glNamedFramebufferTexture(framebufferObject, GL_DEPTH_ATTACHMENT, depthTexture->textureObject, 0);
	}
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &framebufferObject);
}

void Framebuffer::BindFramebuffer(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
}


Drawable::Drawable()
{
	nextDrawable = nullptr;

	indexStart = 0;
	indexCount = 0;

	for (int32 a = 0; a < kMaxTextureCount; a++)
	{
		renderableTexture[a] = nullptr;
	}
}

Drawable::~Drawable()
{
	for (int32 a = kMaxTextureCount - 1; a >= 0; a--)
	{
		if (renderableTexture[a])
		{
			renderableTexture[a]->Release();
		}
	}
}


Renderable::Renderable(int32 type)
{
	renderType = type;
	blendMode = kBlendReplace;

	depthWriteFlag = true;
	cullFaceFlag = true;

	vertexCount = 0;
	textureCount = 0;
	vertexParamLocation = 0;
	vertexParamCount = 0;
	fragmentParamLocation = 0;
	fragmentParamCount = 0;
	transformable = nullptr;

	for (int32 a = 0; a < kMaxVertexBufferCount; a++)
	{
		vertexBuffer[a] = nullptr;
	}

	indexBuffer = nullptr;
	vertexArray = nullptr;

	for (int32 a = 0; a < kMaxProgramCount; a++)
	{
		renderableProgram[a] = nullptr;
	}
}

Renderable::~Renderable()
{
	for (int32 a = kMaxProgramCount - 1; a >= 0; a--)
	{
		if (renderableProgram[a])
		{
			renderableProgram[a]->Release();
		}
	}

	delete vertexArray;
	delete indexBuffer;

	for (int32 a = kMaxVertexBufferCount - 1; a >= 0; a--)
	{
		delete vertexBuffer[a];
	}
}

void Renderable::RenderColor(int32 programIndex)
{
	static const GLenum blendSrcTable[kMaxProgramCount][kBlendCount] =
	{
		{GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA},
		{GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA}
	};

	static const GLenum blendDstTable[kMaxProgramCount][kBlendCount] =
	{
		{GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ONE},
		{GL_ONE, GL_ONE, GL_ONE}
	};

	if (!renderableProgram[programIndex])
	{
		return;
	}

	glBlendFunc(blendSrcTable[programIndex][blendMode], blendDstTable[programIndex][blendMode]);
	glDepthMask(depthWriteFlag);

	((cullFaceFlag) ? glEnable : glDisable)(GL_CULL_FACE);

	vertexArray->BindVertexArray();
	renderableProgram[programIndex]->BindProgram();

	if (vertexParamCount != 0)
	{
		glProgramUniform4fv(renderableProgram[programIndex]->programObject, vertexParamLocation, vertexParamCount, &vertexParam[0].x);
	}

	if (fragmentParamCount != 0)
	{
		glProgramUniform4fv(renderableProgram[programIndex]->programObject, fragmentParamLocation, fragmentParamCount, &fragmentParam[0].x);
	}

	graphicsManager->UpdateUniversalParams();

	const Drawable *drawable = this;
	do
	{
		for (int32 a = 0; a < textureCount; a++)
		{
			drawable->renderableTexture[a]->BindTexture(a);
		}

		switch (renderType)
		{
			case kTypeTriangleList:

				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
				break;

			case kTypeTriangleStrip:

				glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
				break;

			case kTypeIndexedTriangleList:
			{
				int32 count = drawable->indexCount;
				if (count != 0)
				{
					indexBuffer->BindIndexBuffer();
					glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, reinterpret_cast<char *>(drawable->indexStart * sizeof(uint16)));
				}

				break;
			}
		}

		drawable = drawable->nextDrawable;
	} while (drawable);
}

void Renderable::RenderStructure(void)
{
	((cullFaceFlag) ? glEnable : glDisable)(GL_CULL_FACE);

	vertexArray->BindVertexArray();

	if (vertexParamCount != 0)
	{
		glProgramUniform4fv(graphicsManager->GetStructureProgram()->programObject, vertexParamLocation, vertexParamCount, &vertexParam[0].x);
	}

	const Drawable *drawable = this;
	do
	{
		switch (renderType)
		{
			case kTypeTriangleList:

				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
				break;

			case kTypeTriangleStrip:

				glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
				break;

			case kTypeIndexedTriangleList:

				indexBuffer->BindIndexBuffer();
				glDrawElements(GL_TRIANGLES, drawable->indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<char *>(drawable->indexStart * sizeof(uint16)));
				break;
		}

		drawable = drawable->nextDrawable;
	} while (drawable);
}

void Renderable::RenderShadow(void)
{
	((cullFaceFlag) ? glEnable : glDisable)(GL_CULL_FACE);

	vertexArray->BindVertexArray();

	if (vertexParamCount != 0)
	{
		glProgramUniform4fv(graphicsManager->GetShadowProgram()->programObject, vertexParamLocation, vertexParamCount, &vertexParam[0].x);
	}

	const Drawable *drawable = this;
	do
	{
		switch (renderType)
		{
			case kTypeTriangleList:

				glDrawArrays(GL_TRIANGLES, 0, vertexCount);
				break;

			case kTypeTriangleStrip:

				glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
				break;

			case kTypeIndexedTriangleList:

				indexBuffer->BindIndexBuffer();
				glDrawElements(GL_TRIANGLES, drawable->indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<char *>(drawable->indexStart * sizeof(uint16)));
				break;
		}

		drawable = drawable->nextDrawable;
	} while (drawable);
}


GraphicsManager::GraphicsManager(int32 width, int32 height)
{
	screenWidth = width;
	screenHeight = height;
	viewportWidth = float(width);
	viewportHeight = float(height);
	frameIndex = 0;
}

GraphicsManager::~GraphicsManager()
{
}

LRESULT CALLBACK GraphicsManager::WglWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE)
	{
		const CREATESTRUCT *createStruct = (CREATESTRUCT *) lparam;
		const PIXELFORMATDESCRIPTOR *formatDescriptor = (PIXELFORMATDESCRIPTOR *) createStruct->lpCreateParams;

		HDC deviceContext = GetDC(window);
		int pixelFormat = ChoosePixelFormat(deviceContext, formatDescriptor);
		if (pixelFormat != 0)
		{
			if (SetPixelFormat(deviceContext, pixelFormat, formatDescriptor))
			{
				HGLRC openglContext = wglCreateContext(deviceContext);
				if (wglMakeCurrent(deviceContext, openglContext))
				{
					GLGETPROC(wglCreateContextAttribsARB);
					GLGETPROC(wglChoosePixelFormatARB);
					GLGETPROC(wglSwapIntervalEXT);

					wglMakeCurrent(nullptr, nullptr);
				}

				wglDeleteContext(openglContext);
			}
		}

		ReleaseDC(window, deviceContext);
		return (0);
	}
	
	return (DefWindowProc(window, message, wparam, lparam));
}

bool GraphicsManager::InitWglExtensions(HINSTANCE instance, PIXELFORMATDESCRIPTOR *formatDescriptor)
{
	// We have to create a legacy OpenGL context in order to get pointers to
	// the functions that we need to create a core OpenGL context. Yay, OpenGL!

	static const wchar_t name[] = L"wgl";

	WNDCLASSEXW		windowClass;

	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = &WglWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = instance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = name;
	windowClass.hIconSm = nullptr;

	RegisterClassEx(&windowClass);
	DestroyWindow(CreateWindowExW(0, name, name, WS_POPUP, 0, 0, 32, 32, nullptr, nullptr, instance, formatDescriptor));
	UnregisterClass(name, instance);

	if ((!wglCreateContextAttribsARB) || (!wglChoosePixelFormatARB) || (!wglSwapIntervalEXT))
	{
		return (false);
	}

	return (true);
}

bool GraphicsManager::Initialize(HINSTANCE instance, HWND window)
{
	static const int formatAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, true,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_SUPPORT_OPENGL_ARB, true,
		WGL_DOUBLE_BUFFER_ARB, true,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, true,
		WGL_COLOR_BITS_ARB, 24,
		WGL_ALPHA_BITS_ARB, 8,
		0, 0
	};

	static const int contextAttributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 5,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	PIXELFORMATDESCRIPTOR	formatDescriptor;
	UINT					formatCount;
	int						pixelFormat;

	memset(&formatDescriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
	formatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	formatDescriptor.nVersion = 1;
	formatDescriptor.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE | PFD_GENERIC_ACCELERATED;
	formatDescriptor.cColorBits = 24;
	formatDescriptor.cAlphaBits = 8;

	if (!InitWglExtensions(instance, &formatDescriptor))
	{
		return (false);
	}

	deviceContext = GetDC(window);

	if ((!wglChoosePixelFormatARB(deviceContext, formatAttributes, nullptr, 1, &pixelFormat, &formatCount)) || (formatCount == 0))
	{
		ReleaseDC(window, deviceContext);
		return (false);
	}

	if (!SetPixelFormat(deviceContext, pixelFormat, &formatDescriptor))
	{
		ReleaseDC(window, deviceContext);
		return (false);
	}

	openglContext = wglCreateContextAttribsARB(deviceContext, nullptr, contextAttributes);
	if (!openglContext)
	{
		ReleaseDC(window, deviceContext);
		return (false);
	}

	if (!wglMakeCurrent(deviceContext, openglContext))
	{
		wglDeleteContext(openglContext);
		ReleaseDC(window, deviceContext);
		return (false);
	}

	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(1);
	}

	InitializeOpenGL();
	return (true);
}

void GraphicsManager::Terminate(HWND window)
{
	TerminateOpenGL();

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(openglContext);

	ReleaseDC(window, deviceContext);
}

void GraphicsManager::InitializeOpenGL(void)
{
	// Retrieve pointers to all the functions we need that aren't in opengl32.lib.

	GLGETPROC(glBlendEquation);					GLGETPROC(glBlendFuncSeparate);				GLGETPROC(glMultiDrawArrays);
	GLGETPROC(glMultiDrawElements);				GLGETPROC(glClipControl);
	
	GLGETPROC(glCreateBuffers);					GLGETPROC(glDeleteBuffers);					GLGETPROC(glBindBuffer);
	GLGETPROC(glBindBufferBase);				GLGETPROC(glNamedBufferData);				GLGETPROC(glNamedBufferSubData);
	GLGETPROC(glMapNamedBuffer);				GLGETPROC(glUnmapNamedBuffer);

	GLGETPROC(glCreateVertexArrays);			GLGETPROC(glDeleteVertexArrays);			GLGETPROC(glBindVertexArray);
	GLGETPROC(glEnableVertexArrayAttrib);		GLGETPROC(glDisableVertexArrayAttrib);		GLGETPROC(glVertexArrayAttribFormat);
	GLGETPROC(glVertexArrayVertexBuffer);		GLGETPROC(glVertexArrayAttribBinding);

	GLGETPROC(glCreateTextures);				GLGETPROC(glBindTextures);					GLGETPROC(glTextureStorage2D);
	GLGETPROC(glTextureStorage3D);				GLGETPROC(glTextureStorage2DMultisample);	GLGETPROC(glTextureSubImage2D);
	GLGETPROC(glTextureSubImage3D);				GLGETPROC(glCompressedTextureSubImage2D);	GLGETPROC(glCompressedTextureSubImage3D);
	GLGETPROC(glTextureParameterf);				GLGETPROC(glTextureParameterfv);			GLGETPROC(glTextureParameteri);
	GLGETPROC(glTextureParameteriv);

	GLGETPROC(glCreateFramebuffers);			GLGETPROC(glDeleteFramebuffers);			GLGETPROC(glBindFramebuffer);
	GLGETPROC(glBlitNamedFramebuffer);			GLGETPROC(glNamedFramebufferTexture);		GLGETPROC(glNamedFramebufferTextureLayer);
	GLGETPROC(glNamedFramebufferDrawBuffers);	GLGETPROC(glNamedFramebufferReadBuffer);	GLGETPROC(glClearNamedFramebufferfv);
	
	GLGETPROC(glCreateShader);					GLGETPROC(glDeleteShader);					GLGETPROC(glShaderSource);
	GLGETPROC(glCompileShader);					GLGETPROC(glAttachShader);					GLGETPROC(glCreateProgram);
	GLGETPROC(glDeleteProgram);					GLGETPROC(glLinkProgram);					GLGETPROC(glUseProgram);
	GLGETPROC(glProgramUniform4fv);				GLGETPROC(glGetShaderiv);					GLGETPROC(glGetProgramiv);
	GLGETPROC(glGetShaderInfoLog);				GLGETPROC(glGetProgramInfoLog);
	
	GLGETPROC(glGenQueries);					GLGETPROC(glDeleteQueries);					GLGETPROC(glBeginQuery);
	GLGETPROC(glEndQuery);						GLGETPROC(glGetQueryObjectui64v);

	File vertexShader("Shaders/Passthru.glsl");
	const char *vertexString = vertexShader.GetData();

	// Set up the framebuffer storage for color and depth.

	colorTexture = new Texture(Texture::kTypeMultisample, Texture::kFormatGammaRgba, screenWidth, screenHeight, 1, 1, nullptr);
	depthTexture = new Texture(Texture::kTypeMultisample, Texture::kFormatDepth, screenWidth, screenHeight, 1, 1, nullptr);
	framebuffer = new Framebuffer(colorTexture, depthTexture);

	// Set up the structure buffer.

	structureColorTexture = new Texture(Texture::kTypeRectangle, Texture::kFormatFloat16Rgba, screenWidth, screenHeight, 1, 1, nullptr);
	structureDepthTexture = new Texture(Texture::kTypeRectangle, Texture::kFormatDepth, screenWidth, screenHeight, 1, 1, nullptr);
	structureFramebuffer = new Framebuffer(structureColorTexture, structureDepthTexture);

	File structureFragmentShader("Shaders/Structure.glsl");
	const char *fragmentString = structureFragmentShader.GetData();
	structureProgram = new Program(vertexString, fragmentString);

	// Set up the cube shadow map.

	shadowTexture = new Texture(Texture::kTypeCube, Texture::kFormatDepth, kShadowMapSize, kShadowMapSize, 1, 1, nullptr);
	shadowFramebuffer = new Framebuffer(nullptr, structureDepthTexture);

	File shadowFragmentShader("Shaders/Constant.glsl");
	fragmentString = shadowFragmentShader.GetData();
	shadowProgram = new Program(vertexString, fragmentString);

	// Make an index buffer for quads.

	quadIndexBuffer = new Buffer(32766 * sizeof(Triangle), quadTriangle);

	// Establish the buffer for the universal parameters used by the shaders.

	universalBufferObject = new Buffer(sizeof(UniversalParams));
	ZeroMemory(&universalParams, sizeof(UniversalParams));

	// Initialize some OpenGL state that won't change.

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void GraphicsManager::TerminateOpenGL(void)
{
	delete universalBufferObject;
	delete quadIndexBuffer;

	shadowProgram->Release();
	delete shadowFramebuffer;
	shadowTexture->Release();

	structureProgram->Release();
	delete structureFramebuffer;
	structureDepthTexture->Release();
	structureColorTexture->Release();

	delete framebuffer;
	depthTexture->Release();
	colorTexture->Release();
}

void GraphicsManager::BeginRendering(void)
{
	universalBufferObject->BindUniformBuffer();
	universalUpdateFlag = false;

	glDepthMask(GL_TRUE);
	glViewport(0, 0, screenWidth, screenHeight);
}

void GraphicsManager::EndRendering(void)
{
	SwapBuffers(deviceContext);
	frameIndex++;
}

void GraphicsManager::BeginColorRendering(void)
{
	static const float clearColor[4] = {0.0F, 0.125F, 0.0625F, 0.0F};
	static const float clearDepth = 1.0F;

	framebuffer->BindFramebuffer();

	glClearNamedFramebufferfv(framebuffer->framebufferObject, GL_COLOR, 0, clearColor);
	glClearNamedFramebufferfv(framebuffer->framebufferObject, GL_DEPTH, 0, &clearDepth);

	structureColorTexture->BindTexture(15);
}

void GraphicsManager::EndColorRendering(void)
{
	glBlitNamedFramebuffer(framebuffer->framebufferObject, 0, 0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void GraphicsManager::BeginStructureRendering(void)
{
	static const float clearColor[4] = {0.0F, 0.0F, 65504.0F, 0.0F};
	static const float clearDepth = 1.0F;

	structureFramebuffer->BindFramebuffer();

	glClearNamedFramebufferfv(structureFramebuffer->framebufferObject, GL_COLOR, 0, clearColor);
	glClearNamedFramebufferfv(structureFramebuffer->framebufferObject, GL_DEPTH, 0, &clearDepth);

	structureProgram->BindProgram();

	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask(GL_TRUE);
}

void GraphicsManager::EndStructureRendering(void)
{
}

void GraphicsManager::BeginShadowRendering(void)
{
	shadowFramebuffer->BindFramebuffer();

	shadowProgram->BindProgram();

	glViewport(0, 0, kShadowMapSize, kShadowMapSize);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask(GL_TRUE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3.0F, 3.0F);
}

void GraphicsManager::EndShadowRendering(void)
{
	static const GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};

	framebuffer->BindFramebuffer();
	glNamedFramebufferDrawBuffers(framebuffer->framebufferObject, 1, drawBuffers);

	shadowTexture->BindTexture(14);
	glViewport(0, 0, screenWidth, screenHeight);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void GraphicsManager::SetShadowFace(int32 face)
{
	static const float clearDepth = 1.0F;

	glNamedFramebufferTextureLayer(shadowFramebuffer->framebufferObject, GL_DEPTH_ATTACHMENT, shadowTexture->textureObject, 0, face);
	glNamedFramebufferDrawBuffers(shadowFramebuffer->framebufferObject, 0, nullptr);

	glClearNamedFramebufferfv(shadowFramebuffer->framebufferObject, GL_DEPTH, 0, &clearDepth);
}

void GraphicsManager::UpdateUniversalParams(void)
{
	if (universalUpdateFlag)
	{
		universalUpdateFlag = false;
		glNamedBufferSubData(universalBufferObject->bufferObject, 0, sizeof(UniversalParams), &universalParams);
	}
}
