#include "LWidgets.h"
#ifndef CC_BUILD_WEB
#include "String.h"
#include "Gui.h"
#include "Drawer2D.h"
#include "Launcher.h"
#include "ExtMath.h"
#include "Window.h"
#include "Funcs.h"
#include "LWeb.h"
#include "Platform.h"
#include "LScreens.h"
#include "Input.h"
#include "Utils.h"
#include "LBackend.h"

static int flagXOffset, flagYOffset;
static int oneX, twoX, fourX;
static int oneY, twoY, fourY;

void LWidget_CalcOffsets(void) {
	oneX = Display_ScaleX(1); twoX = oneX * 2; fourX = oneX * 4;
	oneY = Display_ScaleY(1); twoY = oneY * 2; fourY = oneY * 4;

	flagXOffset  = Display_ScaleX(2);
	flagYOffset  = Display_ScaleY(6);
}


/*########################################################################################################################*
*------------------------------------------------------ButtonWidget-------------------------------------------------------*
*#########################################################################################################################*/
static void LButton_DrawBase(struct Context2D* ctx, int x, int y, int width, int height, cc_bool hovered) {
	BitmapCol color = hovered ? Launcher_Theme.ButtonForeActiveColor 
							  : Launcher_Theme.ButtonForeColor;

	if (Launcher_Theme.ClassicBackground) {
		Gradient_Noise(ctx, color, 8,
						x + oneX,       y + oneY,
						width - twoX,   height - twoY);
	} else {
		Gradient_Vertical(ctx, BitmapColor_Offset(color, 8,8,8), BitmapColor_Offset(color, -8,-8,-8),
						  x + oneX,     y + oneY,
						  width - twoX, height - twoY);
	}
}

static void LButton_DrawBorder(struct Context2D* ctx, int x, int y, int width, int height) {
	BitmapCol backColor = Launcher_Theme.ButtonBorderColor;
#ifdef CC_BUILD_IOS
	int xoff = 0; /* TODO temp hack */
#else
	int xoff = oneX;
#endif

	Context2D_Clear(ctx, backColor, 
					x + xoff,         y,
					width - 2 * xoff, oneY);
	Context2D_Clear(ctx, backColor,
					x + xoff,         y + height - oneY,
					width - 2 * xoff, oneY);
	Context2D_Clear(ctx, backColor,
					x,                y + oneY,
					oneX,             height - twoY);
	Context2D_Clear(ctx, backColor,
					x + width - oneX, y + oneY,
					oneX,             height - twoY);
}

static void LButton_DrawHighlight(struct Context2D* ctx, int x, int y, int width, int height, cc_bool hovered) {
	BitmapCol activeColor = BitmapColor_RGB(189, 198, 255);
	BitmapCol color       = Launcher_Theme.ButtonHighlightColor;

	if (Launcher_Theme.ClassicBackground) {
		if (hovered) color = activeColor;

		Context2D_Clear(ctx, color,
						x + twoX,      y + oneY,
						width - fourX, oneY);
		Context2D_Clear(ctx, color,
						x + oneX,      y + twoY,
						oneX,          height - fourY);
	} else if (!hovered) {
		Context2D_Clear(ctx, color,
						x + twoX,      y + oneY,
						width - fourX, oneY);
	}
}

void LButton_DrawBackground(struct Context2D* ctx, int x, int y, int width, int height, cc_bool hovered) {
	LButton_DrawBase(     ctx, x, y, width, height, hovered);
	LButton_DrawBorder(   ctx, x, y, width, height);
	LButton_DrawHighlight(ctx, x, y, width, height, hovered);
}

static void LButton_Draw(void* widget) {
	struct LButton* w = (struct LButton*)widget;
	LBackend_ButtonDraw(w);
}

static void LButton_Hover(void* w, int idx, cc_bool wasOver) {
	/* only need to redraw when changing from unhovered to hovered */
	if (!wasOver) LBackend_MarkDirty(w);
}

static void LButton_Unhover(void* w) {
	LBackend_MarkDirty(w);
}

static const struct LWidgetVTABLE lbutton_VTABLE = {
	LButton_Draw, NULL,
	NULL, NULL,                     /* Key    */
	LButton_Hover, LButton_Unhover, /* Hover  */
	NULL, NULL                      /* Select */
};
void LButton_Init(struct LButton* w, int width, int height, const char* text, const struct LLayout* layouts) {
	w->VTABLE  = &lbutton_VTABLE;
	w->type    = LWIDGET_BUTTON;
	w->layouts = layouts;
	w->tabSelectable = true;
	LBackend_ButtonInit(w, width, height);
	LButton_SetConst(w, text);
}

void LButton_SetConst(struct LButton* w, const char* text) {
	w->text = String_FromReadonly(text);
	LBackend_ButtonUpdate(w);
}


/*########################################################################################################################*
*-----------------------------------------------------CheckboxWidget------------------------------------------------------*
*#########################################################################################################################*/
static void LCheckbox_Draw(void* widget) {
	struct LCheckbox* w = (struct LCheckbox*)widget;
	LBackend_CheckboxDraw(w);
}

static const struct LWidgetVTABLE lcheckbox_VTABLE = {
	LCheckbox_Draw, NULL,
	NULL, NULL, /* Key    */
	NULL, NULL, /* Hover  */
	NULL, NULL  /* Select */
};
void LCheckbox_Init(struct LCheckbox* w, const char* text, const struct LLayout* layouts) {
	w->VTABLE  = &lcheckbox_VTABLE;
	w->type    = LWIDGET_CHECKBOX;
	w->layouts = layouts;
	w->tabSelectable = true;

	w->text = String_FromReadonly(text);
	LBackend_CheckboxInit(w);
}

void LCheckbox_Set(struct LCheckbox* w, cc_bool value) {
	w->value = value;
	LBackend_CheckboxUpdate(w);
}


/*########################################################################################################################*
*------------------------------------------------------InputWidget--------------------------------------------------------*
*#########################################################################################################################*/
void LInput_UNSAFE_GetText(struct LInput* w, cc_string* text) {
	int i;
	if (w->inputType != KEYBOARD_TYPE_PASSWORD) { *text = w->text; return; }

	for (i = 0; i < w->text.length; i++) {
		String_Append(text, '*');
	}
}

static void LInput_Draw(void* widget) {
	struct LInput* w = (struct LInput*)widget;
	LBackend_InputDraw(w);
}

static void LInput_TickCaret(void* widget) {
	struct LInput* w = (struct LInput*)widget;
	LBackend_InputTick(w);
}

static void LInput_Select(void* widget, int idx, cc_bool wasSelected) {
	struct LInput* w = (struct LInput*)widget;
	LBackend_InputSelect(w, idx, wasSelected);
}

static void LInput_Unselect(void* widget, int idx) {
	struct LInput* w = (struct LInput*)widget;
	LBackend_InputUnselect(w);
}

static void LInput_AdvanceCaretPos(struct LInput* w, cc_bool forwards) {
	if (forwards && w->caretPos == -1) return;
	if (!forwards && w->caretPos == 0) return;
	if (w->caretPos == -1 && !forwards) /* caret after text */
		w->caretPos = w->text.length;

	w->caretPos += (forwards ? 1 : -1);
	if (w->caretPos < 0 || w->caretPos >= w->text.length) w->caretPos = -1;
	LBackend_InputUpdate(w);
}

static void LInput_CopyFromClipboard(struct LInput* w) {
	cc_string text; char textBuffer[2048];
	String_InitArray(text, textBuffer);

	Clipboard_GetText(&text);
	String_UNSAFE_TrimStart(&text);
	String_UNSAFE_TrimEnd(&text);

	if (w->ClipboardFilter) w->ClipboardFilter(&text);
	LInput_AppendString(w, &text);
}

/* If caret position is now beyond end of text, resets to -1 */
static CC_INLINE void LInput_ClampCaret(struct LInput* w) {
	if (w->caretPos >= w->text.length) w->caretPos = -1;
}

/* Removes the character preceding the caret in the currently entered text */
static void LInput_Backspace(struct LInput* w) {
	if (!w->text.length || w->caretPos == 0) return;

	if (w->caretPos == -1) {
		String_DeleteAt(&w->text, w->text.length - 1);
	} else {	
		String_DeleteAt(&w->text, w->caretPos - 1);
		w->caretPos--;
		if (w->caretPos == -1) w->caretPos = 0;
	}

	if (w->TextChanged) w->TextChanged(w);
	LInput_ClampCaret(w);
	LBackend_InputUpdate(w);
}

/* Removes the character at the caret in the currently entered text */
static void LInput_Delete(struct LInput* w) {
	if (!w->text.length || w->caretPos == -1) return;

	String_DeleteAt(&w->text, w->caretPos);
	if (w->caretPos == -1) w->caretPos = 0;

	if (w->TextChanged) w->TextChanged(w);
	LInput_ClampCaret(w);
	LBackend_InputUpdate(w);
}

static void LInput_KeyDown(void* widget, int key, cc_bool was) {
	struct LInput* w = (struct LInput*)widget;
	if (key == KEY_BACKSPACE) {
		LInput_Backspace(w);
	} else if (key == KEY_DELETE) {
		LInput_Delete(w);
	} else if (key == INPUT_CLIPBOARD_COPY) {
		if (w->text.length) Clipboard_SetText(&w->text);
	} else if (key == INPUT_CLIPBOARD_PASTE) {
		LInput_CopyFromClipboard(w);
	} else if (key == KEY_ESCAPE) {
		if (w->text.length) LInput_SetString(w, &String_Empty);
	} else if (key == KEY_LEFT) {
		LInput_AdvanceCaretPos(w, false);
	} else if (key == KEY_RIGHT) {
		LInput_AdvanceCaretPos(w, true);
	}
}

static cc_bool LInput_CanAppend(struct LInput* w, char c) {
	switch (w->inputType) {
	case KEYBOARD_TYPE_PASSWORD:
		return true; /* keyboard accepts all characters */
	case KEYBOARD_TYPE_INTEGER:
		return c >= '0' && c <= '9';
	}
	return c >= ' ' && c <= '~' && c != '&';
}

/* Appends a character to the currently entered text */
static CC_NOINLINE cc_bool LInput_Append(struct LInput* w, char c) {
	if (LInput_CanAppend(w, c) && w->text.length < w->text.capacity) {
		if (w->caretPos == -1) {
			String_Append(&w->text, c);
		} else {
			String_InsertAt(&w->text, w->caretPos, c);
			w->caretPos++;
		}
		return true;
	}
	return false;
}

static void LInput_KeyChar(void* widget, char c) {
	struct LInput* w = (struct LInput*)widget;
	cc_bool appended = LInput_Append(w, c);

	if (appended && w->TextChanged) w->TextChanged(w);
	if (appended) LBackend_InputUpdate(w);
}

static void LInput_TextChanged(void* widget, const cc_string* str) {
	struct LInput* w = (struct LInput*)widget;
	LInput_SetText(w, str);
	if (w->TextChanged) w->TextChanged(w);
}

static const struct LWidgetVTABLE linput_VTABLE = {
	LInput_Draw, LInput_TickCaret,
	LInput_KeyDown, LInput_KeyChar, /* Key    */
	NULL, NULL,                     /* Hover  */
	/* TODO: Don't redraw whole thing, just the outer border */
	LInput_Select, LInput_Unselect, /* Select */
	NULL, LInput_TextChanged        /* TextChanged */
};
void LInput_Init(struct LInput* w, int width, const char* hintText, const struct LLayout* layouts) {
	w->VTABLE  = &linput_VTABLE;
	w->type    = LWIDGET_INPUT;
	w->tabSelectable = true;
	w->opaque  = true;
	w->layouts = layouts;
	String_InitArray(w->text, w->_textBuffer);
	
	w->hintText = hintText;
	w->caretPos = -1;
	LBackend_InputInit(w, width);
}

void LInput_SetText(struct LInput* w, const cc_string* text) {
	String_Copy(&w->text, text);
	LInput_ClampCaret(w);
	LBackend_InputUpdate(w);
}

void LInput_ClearText(struct LInput* w) {
	w->text.length = 0;
	w->caretPos    = -1;
	LBackend_InputUpdate(w);
}

void LInput_AppendString(struct LInput* w, const cc_string* str) {
	int i, appended = 0;
	for (i = 0; i < str->length; i++) {
		if (LInput_Append(w, str->buffer[i])) appended++;
	}

	if (appended && w->TextChanged) w->TextChanged(w);
	if (appended) LBackend_InputUpdate(w);
}

void LInput_SetString(struct LInput* w, const cc_string* str) {
	LInput_SetText(w, str);
	if (w->TextChanged) w->TextChanged(w);
}


/*########################################################################################################################*
*------------------------------------------------------LabelWidget--------------------------------------------------------*
*#########################################################################################################################*/
static void LLabel_Draw(void* widget) {
	struct LLabel* w = (struct LLabel*)widget;
	LBackend_LabelDraw(w);
}

static const struct LWidgetVTABLE llabel_VTABLE = {
	LLabel_Draw, NULL,
	NULL, NULL, /* Key    */
	NULL, NULL, /* Hover  */
	NULL, NULL  /* Select */
};
void LLabel_Init(struct LLabel* w, const char* text, const struct LLayout* layouts) {
	w->VTABLE  = &llabel_VTABLE;
	w->type    = LWIDGET_LABEL;
	w->layouts = layouts;

	String_InitArray(w->text, w->_textBuffer);
	LBackend_LabelInit(w);
	LLabel_SetConst(w, text);
}

void LLabel_SetText(struct LLabel* w, const cc_string* text) {
	String_Copy(&w->text, text);
	LBackend_LabelUpdate(w);
	LBackend_LayoutWidget((struct LWidget*)w);
}

void LLabel_SetConst(struct LLabel* w, const char* text) {
	cc_string str = String_FromReadonly(text);
	LLabel_SetText(w, &str);
}


/*########################################################################################################################*
*-------------------------------------------------------LineWidget--------------------------------------------------------*
*#########################################################################################################################*/
static void LLine_Draw(void* widget) {
	struct LLine* w = (struct LLine*)widget;
	LBackend_LineDraw(w);
}

static const struct LWidgetVTABLE lline_VTABLE = {
	LLine_Draw, NULL,
	NULL, NULL, /* Key    */
	NULL, NULL, /* Hover  */
	NULL, NULL  /* Select */
};
void LLine_Init(struct LLine* w, int width, const struct LLayout* layouts) {
	w->VTABLE  = &lline_VTABLE;
	w->type    = LWIDGET_LINE;
	w->layouts = layouts;
	LBackend_LineInit(w, width);
}

#define CLASSIC_LINE_COLOR BitmapColor_RGB(128, 128, 128)
BitmapCol LLine_GetColor(void) {
	return Launcher_Theme.ClassicBackground ? CLASSIC_LINE_COLOR : Launcher_Theme.ButtonBorderColor;
}


/*########################################################################################################################*
*------------------------------------------------------SliderWidget-------------------------------------------------------*
*#########################################################################################################################*/
static void LSlider_Draw(void* widget) {
	struct LSlider* w = (struct LSlider*)widget;
	LBackend_SliderDraw(w);
}

static const struct LWidgetVTABLE lslider_VTABLE = {
	LSlider_Draw, NULL,
	NULL, NULL, /* Key    */
	NULL, NULL, /* Hover  */
	NULL, NULL  /* Select */
};
void LSlider_Init(struct LSlider* w, int width, int height, BitmapCol color, const struct LLayout* layouts) {
	w->VTABLE  = &lslider_VTABLE;
	w->type    = LWIDGET_SLIDER;
	w->color   = color;
	w->opaque  = true;
	w->layouts = layouts;
	LBackend_SliderInit(w, width, height);
}

void LSlider_SetProgress(struct LSlider* w, int progress) {
	if (progress == w->value) return;
	w->value = progress;
	LBackend_SliderUpdate(w);
}


/*########################################################################################################################*
*------------------------------------------------------TableWidget--------------------------------------------------------*
*#########################################################################################################################*/
static void FlagColumn_Draw(struct ServerInfo* row, struct DrawTextArgs* args, struct LTableCell* cell, struct Context2D* ctx) {
	struct Flag* flag = Flags_Get(row);
	if (!flag) return;
	Context2D_DrawPixels(ctx, cell->x + flagXOffset, cell->y + flagYOffset, &flag->bmp);
}

static void NameColumn_Draw(struct ServerInfo* row, struct DrawTextArgs* args, struct LTableCell* cell, struct Context2D* ctx) {
	args->text = row->name;
}
static int NameColumn_Sort(const struct ServerInfo* a, const struct ServerInfo* b) {
	return String_Compare(&b->name, &a->name);
}

static void PlayersColumn_Draw(struct ServerInfo* row, struct DrawTextArgs* args, struct LTableCell* cell, struct Context2D* ctx) {
	String_Format2(&args->text, "%i/%i", &row->players, &row->maxPlayers);
}
static int PlayersColumn_Sort(const struct ServerInfo* a, const struct ServerInfo* b) {
	return b->players - a->players;
}

static void UptimeColumn_Draw(struct ServerInfo* row, struct DrawTextArgs* args, struct LTableCell* cell, struct Context2D* ctx) {
	LTable_FormatUptime(&args->text, row->uptime);
}
static int UptimeColumn_Sort(const struct ServerInfo* a, const struct ServerInfo* b) {
	return b->uptime - a->uptime;
}

static void SoftwareColumn_Draw(struct ServerInfo* row, struct DrawTextArgs* args, struct LTableCell* cell, struct Context2D* ctx) {
	/* last column, so adjust to fit size of table */
	int leftover = cell->table->width - cell->x;
	cell->width  = max(cell->width, leftover);
	args->text   = row->software;
}
static int SoftwareColumn_Sort(const struct ServerInfo* a, const struct ServerInfo* b) {
	return String_Compare(&b->software, &a->software);
}

static struct LTableColumn tableColumns[] = {
	{ "",          15, FlagColumn_Draw,     NULL,                false, false, false },
	{ "Name",     328, NameColumn_Draw,     NameColumn_Sort,     true,  false, true  },
	{ "Players",   73, PlayersColumn_Draw,  PlayersColumn_Sort,  true,  true,  true  },
	{ "Uptime",    73, UptimeColumn_Draw,   UptimeColumn_Sort,   true,  true,  true  },
	{ "Software", 143, SoftwareColumn_Draw, SoftwareColumn_Sort, false, true,  true  }
};


void LTable_FormatUptime(cc_string* dst, int uptime) {
	char unit = 's';

	if (uptime >= SECS_PER_DAY * 7) {
		uptime /= SECS_PER_DAY;  unit = 'd';
	} else if (uptime >= SECS_PER_HOUR) {
		uptime /= SECS_PER_HOUR; unit = 'h';
	} else if (uptime >= SECS_PER_MIN) {
		uptime /= SECS_PER_MIN;  unit = 'm';
	}
	String_Format2(dst, "%i%r", &uptime, &unit);
}

void LTable_GetScrollbarCoords(struct LTable* w, int* y, int* height) {
	float scale;
	if (!w->rowsCount) { *y = 0; *height = 0; return; }

	scale   = w->height / (float)w->rowsCount;
	*y      = Math_Ceil(w->topRow * scale);
	*height = Math_Ceil(w->visibleRows * scale);
	*height = min(*y + *height, w->height) - *y;
}

void LTable_ClampTopRow(struct LTable* w) { 
	if (w->topRow > w->rowsCount - w->visibleRows) {
		w->topRow = w->rowsCount - w->visibleRows;
	}
	if (w->topRow < 0) w->topRow = 0;
}

int LTable_GetSelectedIndex(struct LTable* w) {
	struct ServerInfo* entry;
	int row;

	for (row = 0; row < w->rowsCount; row++) {
		entry = LTable_Get(row);
		if (String_CaselessEquals(w->selectedHash, &entry->hash)) return row;
	}
	return -1;
}

void LTable_SetSelectedTo(struct LTable* w, int index) {
	if (!w->rowsCount) return;
	if (index >= w->rowsCount) index = w->rowsCount - 1;
	if (index < 0) index = 0;

	String_Copy(w->selectedHash, &LTable_Get(index)->hash);
	LTable_ShowSelected(w);
	w->OnSelectedChanged();
}

void LTable_RowClick(struct LTable* w, int row) {
	cc_uint64 now;
	LTable_SetSelectedTo(w, row);
	now = Stopwatch_Measure();

	/* double click on row to join */
	if (Stopwatch_ElapsedMS(w->_lastClick, now) < 1000 && row == w->_lastRow) {
		Launcher_ConnectToServer(&LTable_Get(row)->hash);
	}

	w->_lastRow = LTable_GetSelectedIndex(w);
	w->_lastClick = now;
}

cc_bool LTable_HandlesKey(int key) {
	return key == KEY_UP || key == KEY_DOWN || key == KEY_PAGEUP || key == KEY_PAGEDOWN;
}

static void LTable_KeyDown(void* widget, int key, cc_bool was) {
	struct LTable* w = (struct LTable*)widget;
	int index = LTable_GetSelectedIndex(w);

	if (key == KEY_UP) {
		index--;
	} else if (key == KEY_DOWN) {
		index++;
	} else if (key == KEY_PAGEUP) {
		index -= w->visibleRows;
	} else if (key == KEY_PAGEDOWN) {
		index += w->visibleRows;
	} else { return; }

	w->_lastRow = -1;
	LTable_SetSelectedTo(w, index);
}

static void LTable_MouseDown(void* widget, int idx, cc_bool wasOver) {
	struct LTable* w = (struct LTable*)widget;
	LBackend_TableMouseDown(w, idx);
}

static void LTable_MouseMove(void* widget, int idx, cc_bool wasOver) {
	struct LTable* w = (struct LTable*)widget;
	LBackend_TableMouseMove(w, idx);
}

static void LTable_MouseUp(void* widget, int idx) {
	struct LTable* w = (struct LTable*)widget;
	LBackend_TableMouseUp(w, idx);
}

static void LTable_MouseWheel(void* widget, float delta) {
	struct LTable* w = (struct LTable*)widget;
	w->topRow -= Utils_AccumulateWheelDelta(&w->_wheelAcc, delta);
	LTable_ClampTopRow(w);
	LBackend_MarkDirty(w);
	w->_lastRow = -1;
}

static void LTable_Draw(void* widget) {
	struct LTable* w = (struct LTable*)widget;
	LBackend_TableDraw(w);
}

static const struct LWidgetVTABLE ltable_VTABLE = {
	LTable_Draw,      NULL,
	LTable_KeyDown,   NULL, /* Key    */
	LTable_MouseMove, NULL, /* Hover  */
	LTable_MouseDown, LTable_MouseUp, /* Select */
	LTable_MouseWheel,      /* Wheel */
};
void LTable_Init(struct LTable* w, const struct LLayout* layouts) {
	int i;
	w->VTABLE     = &ltable_VTABLE;
	w->type       = LWIDGET_TABLE;
	w->columns    = tableColumns;
	w->numColumns = Array_Elems(tableColumns);
	w->sortingCol = -1;
	w->opaque     = true;
	w->layouts    = layouts;
	
	for (i = 0; i < w->numColumns; i++) {
		w->columns[i].width = Display_ScaleX(w->columns[i].width);
	}
	LBackend_TableInit(w);
}

void LTable_Reset(struct LTable* w) {
	LBackend_TableMouseUp(w, 0);
	LBackend_TableReposition(w);

	w->topRow     = 0;
	w->rowsCount  = 0;
	w->_wheelAcc  = 0.0f;
	w->sortingCol = -1;
}

static int ShouldShowServer(struct LTable* w, struct ServerInfo* server) {
	return String_CaselessContains(&server->name, w->filter) 
		&& (Launcher_ShowEmptyServers || server->players > 0);
}

void LTable_ApplyFilter(struct LTable* w) {
	int i, j, count;

	count = FetchServersTask.numServers;
	for (i = 0, j = 0; i < count; i++) {
		if (ShouldShowServer(w, Servers_Get(i))) {
			FetchServersTask.servers[j++]._order = FetchServersTask.orders[i];
		}
	}

	w->rowsCount = j;
	for (; j < count; j++) {
		FetchServersTask.servers[j]._order = -100000;
	}

	w->_lastRow = -1;
	LTable_ClampTopRow(w);
	LBackend_TableUpdate(w);
}

static int sortingCol;
static int LTable_SortOrder(const struct ServerInfo* a, const struct ServerInfo* b) {
	int order;
	if (sortingCol >= 0) {
		order = tableColumns[sortingCol].SortOrder(a, b);
		return tableColumns[sortingCol].invertSort ? -order : order;
	}

	/* Default sort order. (most active server, then by highest uptime) */
	if (a->players != b->players) return a->players - b->players;
	return a->uptime - b->uptime;
}

static void LTable_QuickSort(int left, int right) {
	cc_uint16* keys = FetchServersTask.orders; cc_uint16 key;

	while (left < right) {
		int i = left, j = right;
		struct ServerInfo* mid = Servers_Get((i + j) >> 1);

		/* partition the list */
		while (i <= j) {
			while (LTable_SortOrder(mid, Servers_Get(i)) < 0) i++;
			while (LTable_SortOrder(mid, Servers_Get(j)) > 0) j--;
			QuickSort_Swap_Maybe();
		}
		/* recurse into the smaller subset */
		QuickSort_Recurse(LTable_QuickSort)
	}
}

void LTable_Sort(struct LTable* w) {
	if (!FetchServersTask.numServers) return;

	sortingCol = w->sortingCol;
	FetchServersTask_ResetOrder();
	LTable_QuickSort(0, FetchServersTask.numServers - 1);

	LTable_ApplyFilter(w);
	LTable_ShowSelected(w);
}

void LTable_ShowSelected(struct LTable* w) {
	int i = LTable_GetSelectedIndex(w);
	if (i == -1) return;

	if (i >= w->topRow + w->visibleRows) {
		w->topRow = i - (w->visibleRows - 1);
	}
	if (i < w->topRow) w->topRow = i;
	LTable_ClampTopRow(w);
}

BitmapCol LTable_RowColor(struct ServerInfo* entry, int row, cc_bool selected) {
	BitmapCol featSelColor  = BitmapColor_RGB( 50,  53,  0);
	BitmapCol featuredColor = BitmapColor_RGB(101, 107,  0);
	BitmapCol selectedColor = BitmapColor_RGB( 40,  40, 40);

	if (entry && entry->featured) {
		return selected ? featSelColor : featuredColor;
	} else if (entry && selected) {
		return selectedColor;
	}

	if (!Launcher_Theme.ClassicBackground) {
		return BitmapColor_RGB(20, 20, 10);
	} else {
		return (row & 1) == 0 ? Launcher_Theme.BackgroundColor : 0;
	}
}
#endif
