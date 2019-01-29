#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "sk_mmkvstoredbstruct.h"
#include "sk_mmkvstoredbstruct_inline.h"

#include "sk_kvsvrstruct.h"
#include "sk_kvsvrstruct_inline.h"


#include "iSvrkit.h"
using namespace std;
template <class T>
int ToSKBuffer( const T *k,Comm::SKBuffer *skbuf,int iPickleType, int iPickleStatus )
{
	if( Comm::ePickleTypeTlv == iPickleType )
	{
		tlv_iter_t it = { 0 };
		it.top = 1;
		it.mode = 1;
		
		tlv_iter_t::buf_t buf;
		int ret = StructToIter<tlv_iter_t>( *(T *)k,it,buf,0,0 );
		if( ret )
		{
			return ret;
		}
		int size = buf.size();
		uint8_t *lp = (uint8_t*)malloc( size );
		buf.copy( lp );

		skbuf->Attach( lp,size );
		return ret;
	}
	else if( Comm::ePickleTypeProtoBuf == iPickleType )
	{
		pb_iter_t it = { 0 };
		it.top = 1;
		
		pb_iter_t::buf_t buf;
		int ret = StructToIter<pb_iter_t>( *(T *)k,it,buf,0,0 );
		if( ret )
		{
			return ret;
		}
		int size = buf.size();
		uint8_t *lp = (uint8_t*)malloc( size );
		buf.copy( lp );

		skbuf->Attach( lp,size );
		return ret;

	}
	return __LINE__;
}
template <class T>
int FromSKBuffer( Comm::SKBuffer *skbuf,T *k,int iPickleType, int iPickleStatus )

{

	if( Comm::ePickleTypeTlv == iPickleType )
	{
		tlv_iter_t it = { 0 };

		it.value.buf.ptr = skbuf->GetBuffer();
		it.value.buf.len = skbuf->GetLen() ;

		it.mode = 1;
		it.top = 1;

		int ret = StructFromIter<tlv_iter_t>( *k,it );
		return ret;
	}
	else if( Comm::ePickleTypeProtoBuf == iPickleType )
	{
		pb_iter_t it = { 0 };

		it.value.buf.ptr = skbuf->GetBuffer();
		it.value.buf.len = skbuf->GetLen() ;

		it.top = 1;
		it.type = pb_iter_t::eType_LenLimit;

		int ret = StructFromIter<pb_iter_t>( *k,it );
		return ret;

	}
	return __LINE__;
}


long diff( struct timeval &e,struct timeval &s )
{
	return ( e.tv_sec - s.tv_sec ) * 1000 + ( e.tv_usec - s.tv_usec ) / 1000;
}
struct iter_t
{
	char *p;
};

static void Set( tlv_iter_t &it,Comm::SKBuffer &skbuf )
{
	it.value.buf.ptr = skbuf.GetBuffer();
	it.value.buf.len = skbuf.GetLen() ;
	it.top = 1;
	it.mode = 1;

}
inline static int _GetVarNum2( unsigned long long &k,uint8_t *&p,uint8_t *end )
{
	k = 0;
	int s = 0;
	while( p < end )
	{
		k |= ( (unsigned long long)( *p & ~0x80 ) << s ) ;
		if( !( *p & 0x80 ) )
		{
			++p;
			return 0;
		}
		++p;
		s += 7;
	}
	return -1;
}

inline static int _GetVarNum( unsigned long long &k,uint8_t *p,uint8_t *end )
{
	k = 0;
	int s = 0;
	while( p < end )
	{
		k |= ( (unsigned long long)( *p & ~0x80 ) << s ) ;
		if( !( *p & 0x80 ) )
		{
			++p;
			return 0;
		}
		++p;
		s += 7;
	}
	return -1;
}


int main(int argc,char *argv[])
{
	{
		StrKey8_t k ;
		StrKey8_t k1 ;
		memset(&k,0,sizeof(k));
		memset(&k1,0,sizeof(k));
		
		k.iCount = 10;
		k.piSInt32_Ptr = (int*)calloc(k.iCount,sizeof(int));
		k.pllSInt64_Ptr = (long long*)calloc( k.iCount, sizeof(long long));
		k.ptKey5 = (StrKey5_t*)calloc( k.iCount, sizeof(StrKey5_t));
		k.pcChar_Ptr = (char*)malloc( k.iCount + 1 );
		k.pcChar_Ptr[ k.iCount ] = '\0';

		for(int i=0;i<k.iCount;i++)
		{
			k.piSInt32_Ptr[i] = i + 200;
			k.pllSInt64_Ptr[i] = i + 400;

			k.ptKey5[i].tKeys.pcBuff = strdup("sunny");
			k.ptKey5[i].tKeys2.pcBuff = strdup("sunny");


		}
		
		Comm::SKBuffer tobuf;

		int retTo = ToSKBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );
		int retFrom = FromSKBuffer( &tobuf,&k1,Comm::ePickleTypeTlv,0 );
		int df = KvsvrPickle::Diff( &k,&k1);
		int df2 = DiffStruct( k,k1 );
		printf("df2 %d\n",df2);

		printf("tlv retTo %d retFrom %d df %d df2 %d bufsize %d (%s) \n",
			retTo,retFrom,df,df2,tobuf.GetLen(),k1.ptKey5[4].tKeys2.pcBuff);

		tobuf.Reset();

		retTo = ToSKBuffer( &k,&tobuf,Comm::ePickleTypeProtoBuf,0 );
		retFrom = FromSKBuffer( &tobuf,&k1,Comm::ePickleTypeProtoBuf,0 );
		df = KvsvrPickle::Diff( &k,&k1);


		printf("pb retTo %d retFrom %d df %d bufsize %d (%s) \n",
			retTo,retFrom,df,tobuf.GetLen(),k1.ptKey5[4].tKeys2.pcBuff);

		int cnt = 10000;
		long long n = 0;
		struct timeval e,s;
		{
			gettimeofday( &s,0 );
			for(int i=0;i<cnt;i++)
			{
				Comm::SKBuffer tobuf;
				retTo = ToSKBuffer( &k,&tobuf,Comm::ePickleTypeProtoBuf,0 );
				retFrom = FromSKBuffer( &tobuf,&k1,Comm::ePickleTypeProtoBuf,0  );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}
		long long u1 = diff( e,s );
		printf("pb %lld n %lld\n",u1,n );
		n = 0;
		{
			gettimeofday( &s,0 );
			for(int i=0;i<cnt;i++)
			{
				Comm::SKBuffer tobuf;
				retTo = ToSKBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );
				retFrom = FromSKBuffer( &tobuf,&k1,Comm::ePickleTypeTlv,0 );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}	
		u1 = diff( e,s );
		printf("tlv %lld n %lld\n",u1,n );
		n = 0;

		{
			Comm::SKBuffer tobuf;
			retTo = ToSKBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );

			gettimeofday( &s,0 );
			for(int i=0;i<cnt;i++)
			{
				retFrom = FromSKBuffer( &tobuf,&k1,Comm::ePickleTypeTlv,0 );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}	
		u1 = diff( e,s );
		printf("tlv from %lld n %lld\n",u1,n );
		n = 0;

		{
			gettimeofday( &s,0 );
			for(int i=0;i<cnt;i++)
			{
				Comm::SKBuffer tobuf;
				retTo = KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );
				retFrom = KvsvrPickle::FromBuffer( &tobuf,&k1,Comm::ePickleTypeTlv,0 );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}	
		u1 = diff( e,s );
		printf("svrkit tlv %lld n %lld\n",u1,n );
		n = 0;
		{
			gettimeofday( &s,0 );
			Comm::SKBuffer tobuf;
			retTo = KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );

			for(int i=0;i<cnt;i++)
			{
				retFrom = KvsvrPickle::FromBuffer( &tobuf,&k1,Comm::ePickleTypeTlv,0 );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}	
		u1 = diff( e,s );
		printf("svrkit from tlv %lld n %lld\n",u1,n );
		n = 0;

		{
			gettimeofday( &s,0 );
			for(int i=0;i<cnt;i++)
			{
				Comm::SKBuffer tobuf;
				retTo = KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeProtoBuf,0 );
				retFrom = KvsvrPickle::FromBuffer( &tobuf,&k1,Comm::ePickleTypeProtoBuf,0 );
				FreeStruct( k1 );
				n += retTo;
				n += retFrom;
			}
			gettimeofday( &e,0 );
		}	
		u1 = diff( e,s );
		printf("svrkit pb %lld n %lld\n",u1,n );
		n = 0;

		FreeStruct( k1 );

		return 0;
	}
	{
		printf("start StrKey8_t\n");
		StrKey8_t k ;
		memset(&k,0,sizeof(k));
		
		k.iCount = 10;
		k.piSInt32_Ptr = (int*)calloc(k.iCount,sizeof(int));
		k.pllSInt64_Ptr = (long long*)calloc( k.iCount, sizeof(long long));
		k.ptKey5 = (StrKey5_t*)calloc( k.iCount, sizeof(StrKey5_t));
		k.pcChar_Ptr = (char*)malloc( k.iCount + 1 );
		k.pcChar_Ptr[ k.iCount ] = '\0';

		for(int i=0;i<k.iCount;i++)
		{
			k.piSInt32_Ptr[i] = i + 200;
			k.pllSInt64_Ptr[i] = i + 400;

			k.ptKey5[i].tKeys.pcBuff = strdup("sunny");
			k.ptKey5[i].tKeys2.pcBuff = strdup("sunny");


		}
		{
			long long x = 0;
			struct timeval e,s;
			gettimeofday( &s,0 );
			for(int i=0;i<10000;i++)
			{
				Comm::SKBuffer tobuf;
				KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );
				x += tobuf.GetBuffer()[15];

			}
			gettimeofday( &e,0 );
			long long u1 = diff( e,s );

			printf("u1 %lld\n",u1);
			Comm::SKBuffer tobuf;
			KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );

			tlv_iter_t it = { 0 };
			it.top = 1;
			it.mode = 1;

			int ret;

			gettimeofday( &s,0 );
			for(int i=0;i<10000;i++)
			{
				tlv_iter_t::buf_t buf;

				ret = StructToIter<tlv_iter_t>( k,it,buf,1,1);
				int size = buf.size();
				uint8_t *lp = (uint8_t*)malloc( size );
				buf.copy( lp );
				Comm::SKBuffer skb;
				skb.Attach( lp,size );
				x += lp[15];



			}
			gettimeofday( &e,0 );
			long long u2 = diff( e,s );
			printf("u2 %lld x %lld\n",u2,x);

			tlv_iter_t::buf_t buf;
			ret = StructToIter<tlv_iter_t>( k,it,buf,1,1);
			printf("me ret %d\n",ret);
			int size = buf.size();
			uint8_t *lp = (uint8_t*)malloc( size );
			buf.copy( lp );
			{
				uint8_t *lq = lp + 12;
				uint8_t *end = lp + size;
				int i=0;
				while( lq < lp + size  )
				{
					unsigned long long id,len;
					_GetVarNum2(id,lq,lp + size);
					_GetVarNum2(len,lq,lp + size);
					//printf("id %lld len %lld\n", id,len);
					lq += len;
					if( i++ > 30 ) break;

				}
			}


			int cmp = memcmp( tobuf.GetBuffer(),lp,8192);
			printf("SysTo %d %d cmp %d\n",tobuf.GetLen(),size,cmp);
			{
				tlv_header_t *h = (tlv_header_t*)tobuf.GetBuffer();
				printf("mode %d\n",h->cMode);
				uint8_t *lq = (uint8_t*)tobuf.GetBuffer() + sizeof(tlv_header_t);
				uint8_t *end = (uint8_t*)tobuf.GetBuffer() + size;
				int i=0;
				while( lq < end  )
				{
					unsigned long long id,len;
					_GetVarNum2(id,lq,end);
					_GetVarNum2(len,lq,end);
					//printf("id %lld len %lld\n", id,len);
					lq += len;
					if( i++ > 30 ) break;

				}
			}


			Comm::SKBuffer fmbuf;
			fmbuf.Attach( lp,size );
			StrKey8_t k2 = { 0 };
			ret = KvsvrPickle::FromBuffer( &fmbuf, &k2,Comm::ePickleTypeTlv,0 );

			int df = KvsvrPickle::Diff( &k,&k2);
			printf("from ret %d df %d k2.iCount %d\n",ret,df,k2.iCount);
			printf("k2.piSInt32_Ptr[0] %d\n",k2.piSInt32_Ptr[0]);
			printf("k2.piSInt64_Ptr[0] %ld\n",k2.pllSInt64_Ptr[0]);
			printf("k2.ptKey5[i].tKeys.pcBuff (%s)\n",
						k2.ptKey5[0].tKeys.pcBuff);

			StrKey8_t k3 = { 0 };
			ret = KvsvrPickle::FromBuffer( &tobuf, &k3,Comm::ePickleTypeTlv,0 );
			df = KvsvrPickle::Diff( &k,&k3);
			printf("from ret %d df %d\n",ret,df);
		}
	}


	{
		StrKey_t k;//
		memset(&k,0,sizeof(k));
		k.ptKeys = new Comm::SKBuiltinString_t[2];
		k.ptKeys[0].pcBuff = strdup( "sunny");
		k.ptKeys[1].pcBuff = strdup( "sunny");
		k.iCount = 2;
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("str ptr[2] k.size %d\n",skbuf.GetLen());

		StrKey_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );
		printf("---> *[str,str] ret %d diff %d\n",ret,diff);
	

	}
	{
		StrKey2_t k = { { { 0 } } };
		k.tKeys[0].pcBuff = strdup( "sunny");
		k.tKeys[1].pcBuff = strdup( "sunny");
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("str[2] k.size %d\n",skbuf.GetLen());

		StrKey2_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );

		printf("---> [str,str] ret %d diff %d\n",ret,diff);


	}
	{
		StrKey7_t k = {  { 0 } };

		k.tKeys.pcBuff = strdup( "sunny");
		k.tKeys.iLen = strlen( k.tKeys.pcBuff );

		k.tKeys2.pcBuff = strdup( "sunny");
		k.tKeys2.iLen = strlen( k.tKeys2.pcBuff );

		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("buf[2] k.size %d\n",skbuf.GetLen());

		StrKey7_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );

		printf("---> [buf,buf] ret %d diff %d\n",ret,diff);


	}
		
	{
		StrKey4_t k ;
		memset(&k,0,sizeof(k));
		k.llKeys[0] = 1;
		k.llKeys[1] = 10000000;
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("long long[2] k.size %d\n",skbuf.GetLen());

		StrKey4_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );
		printf("---> [long long:%lld,long long:%lld] ret %d diff %d ntonl(1) %d\n",
			k2.llKeys[0],k2.llKeys[1],ret,diff,ntohl(1));


	}
	{
		StrKey6_t k;//
		memset(&k,0,sizeof(k));
		k.llKeys = 10;
		k.llKeys2 = 20;
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("{long long}{long long} k.size %d\n",skbuf.GetLen());

		StrKey6_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );


		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2);
		printf("---> {long long}{long long} ret %d diff %d\n",ret,diff);


	}

	{
		StrKey5_t k;//
		memset(&k,0,sizeof(k));
		k.tKeys.pcBuff = strdup( "sunny");
		k.tKeys2.pcBuff = strdup( "sunny");
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("{str}{str} k.size %d\n",skbuf.GetLen());

		StrKey5_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };
		Set( it,skbuf );
		

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );
		printf("---> {str}{str} ret %d diff %d\n",ret,diff);

	}


	{
		StrKey3_t k ;
		memset(&k,0,sizeof(k));
		k.iCount = 2;
		k.pllKeys = new unsigned long long[ k.iCount ];
		k.pllKeys[0] = 1;
		k.pllKeys[1] = 1;
		Comm::SKBuffer skbuf;
		KvsvrPickle::ToBuffer( &k,&skbuf,1,0 );
		printf("long long ptr[2] k.size %d\n",skbuf.GetLen());

		StrKey3_t k2;
		memset(&k2,0,sizeof(k2));
		tlv_iter_t it = { 0 };

		Set( it,skbuf );
		

		int ret = StructFromIter( k2,it );
		int diff = KvsvrPickle::Diff( &k,&k2 );
		printf("---> *[long,long] ret %d diff %d\n",ret,diff);



	}
	{
		printf("start StrKey8_t\n");
		StrKey8_t k ;
		memset(&k,0,sizeof(k));
		
		k.iCount = 100;
		k.piSInt32_Ptr = (int*)calloc(k.iCount,sizeof(int));
		k.pllSInt64_Ptr = (long long*)calloc( k.iCount, sizeof(long long));
		k.ptKey5 = (StrKey5_t*)calloc( k.iCount, sizeof(StrKey5_t));
		k.pcChar_Ptr = (char*)malloc( k.iCount + 1 );
		k.pcChar_Ptr[ k.iCount ] = '\0';

		for(int i=0;i<k.iCount;i++)
		{
			k.piSInt32_Ptr[i] = i + 200;
			k.pllSInt64_Ptr[i] = i + 400;

			k.ptKey5[i].tKeys.pcBuff = strdup("fuck2");
			k.ptKey5[i].tKeys2.pcBuff = strdup("fuck22");


		}

		pb_iter_t it = { 0 };
		it.top = 1;
		pb_iter_t::buf_t buf;
		int ret = StructToIter( k,it,buf,1,1);

		printf("StructToIter ret %d\n",ret);


		{
			int size = buf.size();
			uint8_t *lp = (uint8_t*)malloc( size );
			buf.copy( lp );
			{
				unsigned long long id = 0;
				_GetVarNum(id,lp,lp + size);
				unsigned long long type = ( id & 0x7 );
				id >>= 3;

				printf("id %lld type %lld\n",id,type);
			}

			Comm::SKBuffer skbuf;
			skbuf.Attach( lp,size );

			Comm::SKBuffer skbuf2;
			KvsvrPickle::ToBuffer( &k,&skbuf2,Comm::ePickleTypeProtoBuf,0 );

			printf("memcmp %d\n",memcmp(skbuf2.GetBuffer(),skbuf.GetBuffer(),75));

			StrKey8_t k2 = { 0 };
			StrKey8_t k3 = { 0 };
			int ret = KvsvrPickle::FromBuffer( &skbuf, &k2,Comm::ePickleTypeProtoBuf,0 );
			int ret3 = KvsvrPickle::FromBuffer( &skbuf2, &k3,Comm::ePickleTypeProtoBuf,0 );
			int diff = -1,diff3 = -1;

			diff3 = KvsvrPickle::Diff( &k,&k3 );
			diff = KvsvrPickle::Diff( &k,&k2 );
			printf("from size %d ret %d diff %d\n",size,ret,diff);
			printf("from size %d ret %d diff %d\n",skbuf2.GetLen(),ret3,diff3);

			{
				Comm::SKBuffer tobuf;
				KvsvrPickle::ToBuffer( &k,&tobuf,Comm::ePickleTypeTlv,0 );

				tlv_iter_t it = { 0 };
				it.top = 1;
				tlv_iter_t::buf_t buf;
				int ret = StructToIter( k,it,buf,1,1);
				printf("me ret %d\n",ret);
				int size = buf.size();
				uint8_t *lp = (uint8_t*)malloc( size );
				buf.copy( lp );

				printf("SysTo %d %d\n",tobuf.GetLen(),size);

			}
			return 0;


		//printf("(%s)\n",k2.ptKey5->tKeys.pcBuff);

		}


	}

	{
		printf("start StrKey8_t\n");
		StrKey8_t k ;
		memset(&k,0,sizeof(k));

		k.iCount = 100;
		k.piSInt32_Ptr = (int*)calloc(k.iCount,sizeof(int));
		k.pllSInt64_Ptr = (long long*)calloc( k.iCount, sizeof(long long));
		k.ptKey5 = (StrKey5_t*)calloc( k.iCount, sizeof(StrKey5_t));
		k.pcChar_Ptr = (char*)malloc( k.iCount + 1 );
		k.pcChar_Ptr[ k.iCount ] = '\0';

		for(int i=0;i<k.iCount;i++)
		{
			k.piSInt32_Ptr[i] = i + 200;
			k.pllSInt64_Ptr[i] = i + 400;

			k.ptKey5[i].tKeys.pcBuff = strdup("fuck2");
			k.ptKey5[i].tKeys2.pcBuff = strdup("fuck22");


		}

		
		
		int cnt = 10000;
		long long x1 = 0;
		long long x2 = 0;

		long u1 = 0;
		long u2 = 0;

		struct timeval e,s;
		gettimeofday( &s,0 );

		Comm::SKBuffer buf1;
		for(int i=0;i<cnt;i++)
		{
			pb_iter_t it = { 0 };
			it.top = 1;
			pb_iter_t::buf_t buf;
			StructToIter( k,it,buf,1,1);
			{
				int size = buf.size();
				uint8_t *lp = (uint8_t*)malloc( size );
				buf.copy( lp );
				x1 += lp[10];

				free(lp);
			}
		}
		gettimeofday( &e,0 );
		u1 = diff( e,s );

		Comm::SKBuffer buf;
		gettimeofday( &s,0 );
		for(int i=0;i<cnt;i++)
		{
			//KvsvrPickle::ToBuffer( &k,&buf,2,0 );
			int ret = KvsvrPickle::ToBuffer( &k,&buf,2,0 );
			x2 += buf.GetBuffer()[10];
			buf.Reset();
		
		}
		gettimeofday( &e,0 );
		u2 = diff( e,s );

		printf("x1 %lld x2 %lld used %ld %ld\n",
					x1,x2,u1,u2);



	}
	{
		printf("start StrKey8_t\n");
		StrAndHashKey_t k;
		memset(&k,0,sizeof(k));
		
	
		k.pcStrKey = strdup("userattr_aaa");
		k.llHashKey = 10000;
		k.iToken = 1000;
		k.iId = 1000;

		int cnt = 100000;
		long long x1 = 0;
		long long x2 = 0;

		long u1 = 0;
		long u2 = 0;

		struct timeval e,s;
		gettimeofday( &s,0 );

		Comm::SKBuffer buf1;
		for(int i=0;i<cnt;i++)
		{
			pb_iter_t it = { 0 };
			it.top = 1;
			pb_iter_t::buf_t buf;
			StructToIter( k,it,buf,1,1);
			{
				int size = buf.size();
				uint8_t *lp = (uint8_t*)malloc( size );
				buf.copy( lp );

				free(lp);
			}
		}
		gettimeofday( &e,0 );
		u1 = diff( e,s );

		Comm::SKBuffer buf;
		gettimeofday( &s,0 );
		for(int i=0;i<cnt;i++)
		{
			KvsvrPickle::ToBuffer( &k,&buf,2,0 );
			x2 += buf.GetBuffer()[10];
			buf.Reset();
		
		}
		gettimeofday( &e,0 );
		u2 = diff( e,s );

		printf("x1 %lld x2 %lld used %ld %ld\n",
					x1,x2,u1,u2);



	}



	return 0;
	if( 0 )
	{
		unsigned short u = 0;
		u += 6;
		printf("-u %u\n",u);
		u += 36;
		printf("-u %u\n",u);

		u += 65531;
		printf("-u %u\n",u);
		u += 20;
		printf("-u %u\n",u);

		//u += 65346;
		//printf("-u %u\n",u);
		u += 580;
		printf("-u %u\n",u);

		printf("u1 %u\n",u);


		u = 0;
		u += 20;
		u += 65346;
		u += 580;

		printf("u %u\n",u);

		


		StrKey_t keys = { 0 };

		keys.iCount = 2;
		keys.ptKeys = new Comm::SKBuiltinString_t[ keys.iCount ];

		for(uint32_t i=0;i<keys.iCount;i++)
		{
			keys.ptKeys[i].pcBuff = strdup( "sunny" ) ;
		}

		Comm::SKBuffer skbuf;

		KvsvrPickle::ToBuffer( &keys,&skbuf,1,0 );

		tlv_iter_t it = { 0 };
		it.value.buf.ptr = skbuf.GetBuffer();
		it.value.buf.len = skbuf.GetLen() ;
		it.mode = 1;


		printf("FromBuffer: %d\n",it.value.buf.len);
		StrKey_t keys2 = { 0 };
		int ret = StructFromIter<tlv_iter_t>( keys2,it );
		printf("ret %d\n",ret);

		for(uint32_t i=0;i<keys.iCount;i++)
		{
			printf("keys.iCount2 %d keys.ptKeys2[%d] = %s\n",
					keys2.iCount,i,keys2.ptKeys[i].pcBuff);
		}

		//return 0;
	}

	{
		QRSyncVersion_t ver = { { 0 }};
		//strcpy(ver.sKey,"sunny");
		ver.cKeyLen = 8;
		for(int i=0;i<8;i++)
		{
			ver.hKey[i] = i;
		}
		ver.tKey2[0].tKeys.iCount = 1;
		ver.tKey2[0].tKeys.pllKeys = new unsigned long long[1];
		ver.tKey2[0].tKeys.pllKeys[0] = 1000000;

		Comm::SKBuffer skbuf,pbbuf;
		int ret = KvsvrPickle::ToBuffer( &ver,&skbuf,1,0 );
		int ret2 = KvsvrPickle::ToBuffer( &ver,&pbbuf, 2,0 );
		printf("ret %d ret2 %d\n",ret,ret2);

		QRSyncVersion_t ver1 = {{ 0 }},ver2 = {{ 0 }};

		{
			tlv_iter_t it = { 0 };
			
			Set( it,skbuf);
			
			printf("StructFromIter:\n");
			ret = StructFromIter<tlv_iter_t>( ver1,it );
		
		}
		#if 0
		{
			pb_iter_t it = { 0 };
			it.value.buf.ptr = pbbuf.GetBuffer();
			it.value.buf.len = pbbuf.GetLen() ;
			it.type = pb_iter_t::eType_LenLimit;

			ret2 = StructFromIter<pb_iter_t>( ver2,it );

		}
		#endif
		printf("ver1.cKeyLen %d ver2.cKeyLen %d\n",
				ver1.cKeyLen,ver2.cKeyLen);

		printf("hKey %p\n",ver1.hKey);
		for(int i=0;i<10;i++)
		{
			printf("ver1.hKey[%d] %d\n",i,ver1.hKey[i]);
		}
		printf("----\n");
		int diff1 = KvsvrPickle::Diff( &ver,&ver1 );
		printf("----\n");
		int diff2 = KvsvrPickle::Diff( &ver,&ver2 );
		printf("%d ret %d ret2 %d diff1 %d diff2 %d\n",__LINE__,ret,ret2,diff1,diff2);
		
	}
	return 0;
}
#if 0
	{
		MMStrAndHashKey_t k = { 0 };
		Comm::SKBuffer skbuf;
		MMKVStoreDBPickle::ToBuffer( &k,&skbuf,1,0);

		MMStrAndHashKey_t k2 = { 0 };

		tlv_iter_t it = { 0 };

		it.value.buf.ptr = (uint8_t*)skbuf.GetBuffer();
		it.value.buf.len = skbuf.GetLen();
		it.mode = 1;

		int ret = StructFromIter<tlv_iter_t>( k2,it );
		if( ret )
		{
			printf("ret %d\n",ret);
			return 0;
		}

		FreeStruct( k2);

		//return 0;
	}
	printf("\n-----------------------------\n");
	static char buf[1024 * 1024];
	int buflen = 0;
	FILE *fp = fopen( argv[1],"r" );
	if( fp )
	{
		buflen = fread( buf,1,sizeof(buf),fp );
		fclose( fp );
	}
	if( buflen > 0 )
	{
		char *value = buf;
		int len = buflen;
		int cnt = atoi( argv[2] );
		g_checksum = atoi( argv[3] );
		long long k = 0;
		struct timeval s,e;
		vector<void *> v;
		v.resize( cnt * 3 );

		MMUserAttr_t attr = { 0 };

		tlv_iter_t it = { 0 };
		it.value.buf.ptr = (uint8_t*)value;
		it.value.buf.len = len;

		int ret = StructFromIter<tlv_iter_t>( attr,it );
		
		Comm::SKBuffer sk;
		int ret2 = MMKVStoreDBPickle::ToBuffer( &attr,&sk,1,0);
		value = (char*)sk.GetBuffer();
		len = sk.GetLen();

		Comm::SKBuffer pbbuf;
		ret2 = MMKVStoreDBPickle::ToBuffer( &attr,&pbbuf,2,0);

//0.
		gettimeofday(&s,NULL);
		k = 0;
		for(int i=0;i<cnt/1000;i++)
		{
			MMUserAttr_t *attr = (MMUserAttr_t*)calloc( 1,sizeof(MMUserAttr_t) );			
    		
			v.push_back( attr );

			int ret = MMKVStoreDBPickle::FromBuffer( &sk, attr,1,0 );
			if( ret )
			{
				printf("FromBuffer ret %d\n",ret);
			}

			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);
		printf("Pickle::FromBuffer tlv used %ld ms k %lld\n",
				diff( e,s ),k);
//0.1
		gettimeofday(&s,NULL);
		k = 0;
		for(int i=0;i<cnt/1000;i++)
		{
			MMUserAttr_t *attr = (MMUserAttr_t*)calloc( 1,sizeof(MMUserAttr_t) );			
			v.push_back( attr );
			int ret = MMKVStoreDBPickle::FromBuffer( &pbbuf, attr,2,0 );
			if( ret )
			{
				printf("FromBuffer ret %d\n",ret);
			}
			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);
		printf("Pickle::FromBuffer pb used %ld ms k %lld\n",
				diff( e,s ),k);


//1.
		g_checksum_len = 0;
		gettimeofday(&s,NULL);
		k = 0;
		for(int i=0;i<cnt;i++)
		{
			MMUserAttr_t *attr = (MMUserAttr_t*)calloc( 1,sizeof(MMUserAttr_t) );			
			v.push_back( attr );
			tlv_iter_t it = { 0 };

			it.value.buf.ptr = (uint8_t*)value;
			it.value.buf.len = len;
			it.mode = 1;

			int ret = StructFromIter<tlv_iter_t>( *attr,it );
			k += ret;
			if( ret )
			{
				printf("ret %d\n",ret);
				return 0;
			}

			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);

		printf("code tlv used %ld ms k %lld avg checklen %.2f\n",
				diff( e,s ),k,
				double(g_checksum_len)/cnt);
//1.
		gettimeofday(&s,NULL);
		k = 0;
		for(int i=0;i<cnt;i++)
		{
			char *lp = (char*)malloc( len );	
			memcpy( lp,value,len );
			k += _GetCheckSum( (uint8_t*)lp,len );
		}
		gettimeofday(&e,NULL);
		printf("pure checksum used %ld ms k %lld\n",
				diff( e,s ),k);

//2.
		k = 0;

		it.value.buf.ptr = (uint8_t*)value;
		it.value.buf.len = len;
		it.mode = 1;

		FreeStruct( attr );
		ret = StructFromIter<tlv_iter_t>( attr,it );

		
		printf("ret %d\n",ret);
		Comm::SKBuffer skbuf;
		MMKVStoreDBPickle::ToBuffer( &attr,&skbuf,1,0);

		printf("frombuff %d tobuff %d pbsize %d len %d\n",ret,ret2,pbbuf.GetLen() ,len);

		gettimeofday(&s,NULL);
		g_checksum_len = 0;
		for(int i=0;i<cnt;i++)
		{
			MMUserAttr_t *attr = (MMUserAttr_t*)calloc( 1,sizeof(MMUserAttr_t) );			
			v.push_back( attr );
			pb_iter_t it = { 0 };
			it.value.buf.ptr = pbbuf.GetBuffer();
			it.value.buf.len = pbbuf.GetLen() ;
			it.type = pb_iter_t::eType_LenLimit;

			int ret = StructFromIter<pb_iter_t>( *attr,it );
			k += ret;
			if( ret )
			{
				printf("ret %d\n",ret);
				return 0;
			}

			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);
		printf("code pb used %ld ms k %lld checklen %d\n", diff( e,s ), k, g_checksum_len / cnt);

		MMUserAttr_t at = { 0 };
		MMUserAttr_t at2 = { 0 };
		{
			pb_iter_t it = { 0 };
			it.value.buf.ptr = pbbuf.GetBuffer();
			it.value.buf.len = pbbuf.GetLen() ;
			it.type = pb_iter_t::eType_LenLimit;

			tlv_iter_t it2 = { 0 };
			it2.value.buf.ptr = skbuf.GetBuffer();
			it2.value.buf.len = skbuf.GetLen() ;

			int ret = StructFromIter<pb_iter_t>( at,it );
			int ret2 = StructFromIter<tlv_iter_t>( at2,it2 );
			int diff = MMKVStoreDBPickle::Diff( &at,&at2 );
			printf("ret %d ret2 %d diff %d\n",ret,ret2,diff);

			printf("at.llSnsBGObjectID %llx at2.llSnsBGObjectID %llx\n",
					at.llSnsBGObjectID,at2.llSnsBGObjectID);
			FreeStruct( at );
			FreeStruct( at2 );
			
		}


//3.
		gettimeofday(&s,NULL);
		k = 0;
		unsigned long long ll = 0;
		for(int i=0;i<cnt;i++)
		{
			char *lp = (char*)malloc( len );
			memcpy( lp,value,len );
			ll += len;
			k += lp[ i % len ];
			v.push_back( lp );
			//free( lp );
		}

		gettimeofday(&e,NULL);
		printf("memcpy len %lld single %d used %ld ms k %lld\n",
				ll,len,diff( e,s ),k);
		for(size_t i=0;i<v.size();i++)
		{
			free( v[i] );
		}
		FreeStruct( attr );


		
		MMStrAndHashKey_t key = { 0 };	
		key.pcStrKey = strdup("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		key.llHashKey = 10000000;
		key.iToken = 100;
		key.iId = 0;

		skbuf.Reset();
		pbbuf.Reset();
		MMKVStoreDBPickle::ToBuffer( &key,&pbbuf,2,0);
		MMKVStoreDBPickle::ToBuffer( &key,&skbuf,1,0);


		gettimeofday(&s,NULL);
		for(int i=0;i<cnt;i++)
		{
			MMStrAndHashKey_t *attr = (MMStrAndHashKey_t*)calloc( 1,sizeof(MMStrAndHashKey_t) );			
			v.push_back( attr );
			pb_iter_t it = { 0 };
			it.value.buf.ptr = pbbuf.GetBuffer();
			it.value.buf.len = pbbuf.GetLen() ;
			it.type = pb_iter_t::eType_LenLimit;

			int ret = StructFromIter<pb_iter_t>( *attr,it );
			k += ret;
			if( ret )
			{
				printf("ret %d\n",ret);
				return 0;
			}
			k += ret;

			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);
		printf("MMStrAndHashKey_t pb_code used %ld ms k %lld\n",
				diff( e,s ),k);

		gettimeofday(&s,NULL);
		for(int i=0;i<cnt;i++)
		{
			MMStrAndHashKey_t *attr = (MMStrAndHashKey_t*)calloc( 1,sizeof(MMStrAndHashKey_t) );			
			v.push_back( attr );
			tlv_iter_t it = { 0 };
			it.value.buf.ptr = skbuf.GetBuffer();
			it.value.buf.len = skbuf.GetLen() ;
			it.mode = 1;

			int ret = StructFromIter<tlv_iter_t>( *attr,it );
			k += ret;
			if( ret )
			{
				printf("ret %d\n",ret);
				return 0;
			}
			k += ret;

			FreeStruct( *attr );
		}
		gettimeofday(&e,NULL);
		printf("MMStrAndHashKey_t tlv_code used %ld ms k %lld\n",
				diff( e,s ),k);


	}
	return 0;

}
#endif
