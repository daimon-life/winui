#ifndef __XINCONTAINER_H__
#define __XINCONTAINER_H__

#pragma once

namespace DuiLib{
	class CHorXContainerLayout : public CHorizontalLayoutUI
	{
	public:
		CHorXContainerLayout(void);
		~CHorXContainerLayout(void);

		void SetPos(RECT rc);
	};



	class CVerXContainerLayout : public CVerticalLayoutUI
	{
	public:
		CVerXContainerLayout(void);
		~CVerXContainerLayout(void);
		void SetPos(RECT rc);
	};

}

#endif