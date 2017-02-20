// This file is part of retro-gtk. License: GPLv3

public enum Retro.VideoFilter {
	SMOOTH,
	SHARP;

	public static VideoFilter from_string (string filter) {
		var enumc = (EnumClass) typeof (VideoFilter).class_ref ();
		var eval = enumc.get_value_by_nick (filter);

		return eval == null ? VideoFilter.SMOOTH : (VideoFilter) eval.value;
	}
}
