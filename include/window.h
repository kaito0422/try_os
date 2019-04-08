void new_win(struct SHEET *sht, int x_size, int y_size, unsigned char *title, int act);
void win_load_character(struct SHEET *sht, int x, int y, char color, char back_color, unsigned char k);
void win_load_str(struct SHEET *sht, int x, int y, char color, char back_color, unsigned char *str);
void win_load_int(struct SHEET *sht, int x, int y, char color, char back_color, int k);
void reload_win(struct SHEET *sheet, int act);