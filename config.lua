-- config.lua

config = {
	-- application metadata
	app = {
		title = "sudoku",
		version = "0.1.0"
	},

	-- grid definitions
	grid = {
		board_size = 9,
		subgrid = 3
	},

	-- window settings
	window = {
		tile_pix = 76,
		grid_line_thick = 3,
		grid_line_thick_bold = 6,
		board_pad = 28,
		sidebar_w = 340,
		topbar_h = 48
	},

	-- ux settings
	ux = {
		repeat_delay_frames = 22,
		repeat_rate_frames = 5
	},

	-- ui layout constants
	ui = {
		menu_start_y = 200,
		menu_item_spacing = 56,
		menu_padding_x = 16,
		menu_padding_y = 8,
		topbar_padding = 16,
		sidebar_margin = 16,
		controls_line_spacing = 22,
		controls_section_spacing = 28,
		color_keypad_size = 32,
		color_keypad_spacing = 8,
		color_keypad_cols = 3
	},

	-- font sizes
	font = {
		size_title = 48,
		size_digit = 36,
		size_menu = 32,
		size_heading = 24,
		size_large = 22,
		size_topbar = 20,
		size_normal = 18,
		size_small = 16,
		size_note = 14
	},

	-- cell layout
	cell = {
		note_padding_x = 6,
		note_padding_y = 4,
		note_grid_size = 3
	},

	-- theme colors (rgba format: 0xRRGGBBAA)
	theme = {
		bg = 0xFFFFFFFF,
		grid = 0x000000FF,
		grid_bold = 0x000000FF,
		cell_bg = 0xFFFFFFFF,
		cell_sel = 0x2B5CFF40,
		digit_given = 0x000000FF,
		digit_user = 0x0066FFFF,
		accent = 0x2B5CFFFF,
		text = 0x000000FF,
		bad = 0xFF4D4DFF,
		topbar_bg = 0xF0F0F0FF,
		topbar_text = 0x000000FF,
		menu_text = 0x333333FF,
		menu_sel = 0x2B5CFFFF,
		menu_sel_bg = 0x2B5CFF30,
		highlight_row_col = 0x2B5CFF15,
		highlight_digit = 0xFFD70025,

		-- cell color palette (semi-transparent backgrounds)
		palette = {
			red = 0xFF6B6B80,
			orange = 0xFFA50080,
			yellow = 0xFFEB3B80,
			green = 0x4CAF5080,
			blue = 0x2196F380,
			indigo = 0x3F51B580,
			violet = 0x9C27B080,
			light_gray = 0xBDBDBD80,
			white = 0xFFFFFF80
		}
	}
}

