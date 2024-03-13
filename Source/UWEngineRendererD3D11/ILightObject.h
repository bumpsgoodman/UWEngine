/*
* LightObject Interface
*
* 작성자: bumpsgoodman
* 작성일: 2023.03.13
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "IMeshObject.h"

interface ILightObject : public IMeshObject
{
    virtual void __stdcall SetColor(const ColorF color) = 0;
    virtual ColorF __stdcall GetColor() const = 0;
};