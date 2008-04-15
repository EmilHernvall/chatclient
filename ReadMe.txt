	/*case WM_ERASEBKGND:
		hdc = (HDC)wParam;
		GetClientRect(hWnd, &rcArea);

		hBg = LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, 0);
		if (hBg == NULL) {
			ErrorExit(TEXT("LoadImage"));
		}

		hdcSource = CreateCompatibleDC(hdc);
		SelectObject(hdcSource, hBg);

		GetObject(hBg, sizeof(BITMAP), &bmp);
		StretchBlt(hdc, rcArea.left, rcArea.top, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top, 
			hdcSource, 0, 0, bmp.bmWidth, bmp.bmHeight, MERGECOPY);

		brush.lbColor = RGB(255, 0, 0);
		brush.lbHatch = 0;
		brush.lbStyle = BS_SOLID;
		FillRect(hdc, &rcArea, CreateBrushIndirect(&brush));
		return 1;*/