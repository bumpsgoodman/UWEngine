/*
* MeshObject
*
* 작성자: bumpsgoodman
* 작성일: 2023.03.13
*/

#include "Precompiled.h"
#include "ILightObject.h"

class LightObject : public ILightObject
{
public:
    LightObject() = default;
    LightObject(const LightObject&) = delete;
    LightObject& operator=(const LightObject&) = delete;
    ~LightObject() = default;


};