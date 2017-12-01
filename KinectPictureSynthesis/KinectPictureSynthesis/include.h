#pragma once

#include "imgproc.hpp"
#include "highgui.hpp"

#include <Kinect.h>
#include <Windows.h>

#include <math.h>       

using   namespace   cv;

template<class Interface>
inline void SafeRelease(Interface *&pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}