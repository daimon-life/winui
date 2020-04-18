#if !defined(__MICROSOFT_GIDPLUS_BY_JADE_)
#define  __MICROSOFT_GIDPLUS_BY_JADE_

#include <gdiplus.h>
using namespace Gdiplus;

#define  GetImageInfo(r,n,t) do{ UINT num=0,size=0;											\
							   if( Ok == GetImage##t##Size(&num,&size) )					\
							   {  if( r = (ImageCodecInfo*)(malloc(size)) )					\
								  {	if( Ok != GetImage##t##(num,size,r))					\
									{	free(r);r = 0;num=0;	}							\
									else													\
									{   n = num;	}										\
								  }															\
								}															\
							}while(0)
class MsGdiplus
{
public:
	static MsGdiplus* newInstance(void)
	{
		if( pGdiplus == 0 )
			pGdiplus = new MsGdiplus();
		return pGdiplus;
	}
	static void	Release(void)
	{
		if( pGdiplus )
		{
			delete pGdiplus;
			pGdiplus = 0;
		}
	}
	ImageCodecInfo*	getCodecEncodersFirst(void);	//��ȡ�����б�ͷ
	ImageCodecInfo* getCodecDecodersFirst(void);	//��ȡ�����б�ͷ

	ImageCodecInfo*	getCodecEncoders(unsigned int nIndex);		//��ȡ����
	ImageCodecInfo* getCodecDecoders(unsigned int nIndex);		//��ȡ����

	unsigned int	getCodecEncodersTotal(void);
	unsigned int	getCodecDecodersTotal(void);
	bool			DeterminingFormat(const WCHAR *pExtern,bool bEncoders);
protected:
	class SysImageCodec
	{
	public:
		SysImageCodec()
		{
			m_Count			= 0;
			m_pCodecInfo	= 0;
		}
		~SysImageCodec()
		{

		}
		ImageCodecInfo* alloc(unsigned long nSize)
		{
			ImageCodecInfo *pTmp = 0; 
			if( pTmp = (ImageCodecInfo*)malloc(nSize) )
			{
				memset(pTmp,0,nSize);
				if( m_pCodecInfo )
				{
					free(m_pCodecInfo);
					m_Count = 0;
				}
				m_pCodecInfo = pTmp;
			}
			return m_pCodecInfo;
		}
		ImageCodecInfo *m_pCodecInfo;
		unsigned int	m_Count;
	};
	MsGdiplus();
	~MsGdiplus();
private:
	GdiplusStartupInput		m_gdiplusStartupInput;
	GdiplusStartupOutput	m_gdiplusStartupOutput;
	ULONG_PTR				m_gdiplusToken;
	SysImageCodec			m_ImageEncoders;			//������
	SysImageCodec			m_ImageDecoders;			//������
	static MsGdiplus		*pGdiplus;					//��̬ȫ�ֶ���
};


#endif /*__MICROSOFT_GIDPLUS_BY_JADE_*/