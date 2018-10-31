/* To avoid CSS expressions while still supporting IE 7 and IE 6, use this script */
/* The script tag referencing this file must be placed before the ending body tag. */

/* Use conditional comments in order to target IE 7 and older:
	<!--[if lt IE 8]><!-->
	<script src="ie7/ie7.js"></script>
	<!--<![endif]-->
*/

(function() {
	function addIcon(el, entity) {
		var html = el.innerHTML;
		el.innerHTML = '<span style="font-family: \'icomoon\'">' + entity + '</span>' + html;
	}
	var icons = {
		'icon-share': '&#xe600;',
		'icon-home': '&#xe900;',
		'icon-wifi-5g': '&#xe91c;',
		'icon-wifi-24g': '&#xe91d;',
		'icon-bbs': '&#xe96b;',
		'icon-object': '&#xe976;',
		'icon-bandwidth': '&#xe992;',
		'icon-system': '&#xe996;',
		'icon-security': '&#xe998;',
		'icon-stats': '&#xe99b;',
		'icon-download2': '&#xe9c5;',
		'icon-vpn': '&#xe9ca;',
		'icon-circle': '&#xf111;',
		'icon-wizard': '&#xe102;',
		'icon-active': '&#xe603;',
		'icon-auth': '&#xe605;',
		'icon-apmodel': '&#xe601;',
		'icon-setting': '&#xe602;',
		'icon-advanced': '&#xea0c;',
		'0': 0
		},
		els = document.getElementsByTagName('*'),
		i, c, el;
	for (i = 0; ; i += 1) {
		el = els[i];
		if(!el) {
			break;
		}
		c = el.className;
		c = c.match(/icon-[^\s'"]+/);
		if (c && icons[c[0]]) {
			addIcon(el, icons[c[0]]);
		}
	}
}());
