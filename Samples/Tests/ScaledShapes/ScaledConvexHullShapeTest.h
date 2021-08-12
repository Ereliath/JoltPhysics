// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Tests/Test.h>

class ScaledConvexHullShapeTest : public Test
{
public:
	JPH_DECLARE_RTTI_VIRTUAL(ScaledConvexHullShapeTest)

	// See: Test
	virtual void	Initialize() override;
};