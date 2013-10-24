#include "stdafx.h"
#include "..\..\common\Tiles.hpp"

static ITokenManager *tokenManager = NULL;

void ProcessTilePositions()
{
	try
	{
		if (tokenManager == NULL)
			GetTokenManager(&tokenManager);
		if (tokenManager)
		{
#ifdef DEBUG_IN_FILE
			FILE *f = _wfopen(L"\\TileLog.txt", L"at");
#endif

			mlist targetlist;
			Shell::Tiles::CollectSortedTokenListFromRegistry(&targetlist, tokenManager);

			mlist list;
			Shell::Tiles::CollectSortedTokenList(&list);

			// so, we have collected sorted lists of tokens. Let's apply them to actual layout.
			if (targetlist.size() > 0)
			{
				Shell::CTilePosition tp;
				tp.SetDirect((*targetlist.rbegin())->dwPosition);
				int shift = tp.GetRow() + 1;
				TILE_TYPE tileType = tp.GetTileType();
				if (tileType == LARGE || tileType == MEDIUM)
					shift++;
				
				int n = 0;
				for (mlist::reverse_iterator iterator = list.rbegin(); iterator != list.rend(); ++iterator)
				{
					MOVABLE_TILE *mt = *iterator;
#ifdef DEBUG_IN_FILE
					fwprintf(f, L"[ProcessTilePositions] OLD %d: %ls %X\r\n", n, mt->tokenID, mt->dwPosition);
					fflush(f);
#endif
					RETAILMSG(1, (L"[ProcessTilePositions] OLD %d: %ls %X\r\n", n, mt->tokenID, mt->dwPosition + shift));
		
					Shell::Tiles::MoveTokenSimple(mt->appId, mt->tokenID, mt->dwPosition + shift);
					n++;
				}
				// giving StartHost a chance to move tokens right in time.
				Sleep(2000);
				n = 0;
				for (mlist::iterator iterator = targetlist.begin(); iterator != targetlist.end(); ++iterator)
				{
					MOVABLE_TILE *mt = *iterator;
#ifdef DEBUG_IN_FILE
					fwprintf(f, L"[ProcessTilePositions] TARGET %d: %ls %X\r\n", n, mt->tokenID, mt->dwPosition);
					fflush(f);
#endif
					RETAILMSG(1, (L"[ProcessTilePositions] TARGET %d: %ls %X\r\n", n, mt->tokenID, mt->dwPosition));

					Shell::Tiles::MoveTokenSimple(mt->appId, mt->tokenID, mt->dwPosition);
					n++;
				}
			}

#ifdef DEBUG_IN_FILE
			fclose(f);
#endif
			
			// cleanup
			for (mlist::iterator iterator = list.begin(); iterator != list.end(); ++iterator)
			{
				delete(*iterator);
			}
			list.erase(list.begin(), list.end());

			for (mlist::iterator iterator = targetlist.begin(); iterator != targetlist.end(); ++iterator)
			{
				delete(*iterator);
			}
			targetlist.erase(targetlist.begin(), targetlist.end());
			
			Sleep(500);
			Shell::Tiles::FixSpaceBetweenRows(tokenManager);
		}
		Shell::Tiles::FixTokenAbsence();
	}
	catch (...)
	{
	}
}

