#include "renderer.h"

namespace RZUF3_RendererStaticImpl {

	template <typename T>
	void screenToObjectXY(RZUF3_Object* parentObject, T& x, T& y)
	{
		RZUF3_Pos objectPos = parentObject->getAbsolutePos();

		x = (x - objectPos.x) / objectPos.scaleX;
		y = (y - objectPos.y) / objectPos.scaleY;
	}

	template <typename T>
	void objectToScreenXY(RZUF3_Object* parentObject, T& x, T& y)
	{
		RZUF3_Pos parentPos = parentObject->getAbsolutePos();

		x = parentPos.x + x * parentPos.scaleX;
		y = parentPos.y + y * parentPos.scaleY;
	}

	template <typename RectT>
	void objectToScreenRect(RZUF3_Object* parentObject, RectT& rect)
	{
		RZUF3_Pos pos = parentObject->getAbsolutePos();

		rect.x = pos.x + rect.x * pos.scaleX;
		rect.y = pos.y + rect.y * pos.scaleY;
		rect.w *= pos.scaleX;
		rect.h *= pos.scaleY;
	}

	template <typename RectT, typename T>
	void screenToRectXY(RZUF3_Object* parentObject, RectT& rect, T& x, T& y)
	{
		screenToObjectXY(parentObject, x, y);

		x -= rect.x;
		y -= rect.y;
	}

	template <typename RectT, typename T>
	bool isXYInside(RectT& rect, T x, T y, bool ignoreRectXY)
	{
		if (!ignoreRectXY)
		{
			x -= rect.x;
			y -= rect.y;
		}

		if (x < 0 || x >= rect.w) return false;
		if (y < 0 || y >= rect.h) return false;

		return true;
	}

	template <typename RectT>
	void alignRect(RectT& rect, RZUF3_Align alignment, RectT* refRect)
	{
		if (refRect == nullptr) refRect = &rect;

		switch (alignment)
		{
		case RZUF3_Align_TopLeft:
			break;
		case RZUF3_Align_Top:
			rect.x -= refRect->w / 2;
			break;
		case RZUF3_Align_TopRight:
			rect.x -= refRect->w;
			break;
		case RZUF3_Align_Left:
			rect.y -= refRect->h / 2;
			break;
		case RZUF3_Align_Center:
			rect.x -= refRect->w / 2;
			rect.y -= refRect->h / 2;
			break;
		case RZUF3_Align_Right:
			rect.x -= refRect->w;
			rect.y -= refRect->h / 2;
			break;
		case RZUF3_Align_BottomLeft:
			rect.y -= refRect->h;
			break;
		case RZUF3_Align_Bottom:
			rect.x -= refRect->w / 2;
			rect.y -= refRect->h;
			break;
		case RZUF3_Align_BottomRight:
			rect.x -= refRect->w;
			rect.y -= refRect->h;
			break;
		}
	}
};
