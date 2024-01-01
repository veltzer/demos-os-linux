/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <aspell.h>
#include <gcc-plugin.h>
#include <coretypes.h>
#include <diagnostic.h>
#include <gimple.h>
#include <tree.h>
// #include <tree-flow.h>
#include <tree-pass.h>

/* Must declare this */
int plugin_is_GPL_compatible=1;
/* The speller */
static AspellSpeller *speller_g;
/* Do you want to publicize the info about this plugin? */
//#define DO_INFO

/* Help info about the plugin if one were to use gcc's --version --help */
#ifdef DO_INFO
static struct plugin_info speller_info =
{
	.version = "42",
	.help = "Hahahaha yeaaaaa....",
};
#endif // DO_INFO

static struct plugin_gcc_version speller_ver =
{
	.basever = VER,
};

/* We don't need to run any tests before we execute our plugin pass */
static bool speller_gate(void) {
	return true;
}

static const_tree is_str_cst(const_tree node) {
	const_tree str = node;
	/* Filter out types we are ignoring */
	if (TREE_CODE(str) == VAR_DECL) {
		if (!(str = DECL_INITIAL(node)))
			return NULL_TREE;
		else if (TREE_OPERAND_LENGTH(str))
			str = TREE_OPERAND(str, 0);
	} else if (TREE_CODE(str) == ADDR_EXPR && TREE_OPERAND_LENGTH(str) > 0)
		str = TREE_OPERAND(str, 0);
	if (TREE_CODE(str) != STRING_CST && TREE_OPERAND_LENGTH(str) > 0)
		str = TREE_OPERAND(str, 0);
	if (TREE_CODE(str) != STRING_CST)
		return NULL_TREE;
	else
		return str;
}

static AspellSpeller *init_spellchecker(void) {
	AspellConfig *cfg;
	AspellCanHaveError *err;

	/* Configure and instantiate a spell checker */
	cfg = new_aspell_config();
	aspell_config_replace(cfg, "lang", "en_US");
	err = new_aspell_speller(cfg);
	if (aspell_error_number(err) != 0) {
		puts(aspell_error_message(err));
		return NULL;
	}
	return to_aspell_speller(err);
}

static void spell_check(const_gimple stmt, const_tree str) {
	char buf[32] = {0};
	const char *data, *end;

	data = TREE_STRING_POINTER(str);
	printf("Spell checking string: \'%s\'\n", data);

	while (*data) {
		/* Skip non alphas including whitespace */
		while (!ISALPHA(data[0])) {
			if (data[0] == '\0')
				return;
			++data;
		}

		/* Find the end of the word */
		end = data;
		while (ISALPHA(end[0]))
			++end;

		if ((end - data) > (int)sizeof(buf))
			return;

		memcpy(buf, data, end - data);
		buf[end-data] = '\0';
		if (!(aspell_speller_check(speller_g, buf, end - data)))
			warning_at(gimple_location(stmt), 0, "%s (bad spelling)", buf);
		data = end;
	}
}

static unsigned speller_exec(void) {
	unsigned i;
	const_tree str, op;
	basic_block bb;
	gimple stmt;
	gimple_stmt_iterator gsi;

	FOR_EACH_BB(bb)
		for(gsi=gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
			stmt = gsi_stmt(gsi);
			for(i=0; i<gimple_num_ops(stmt); ++i)
				if ((op = gimple_op(stmt, i)) && (str = is_str_cst(op)))
					spell_check(stmt, str);
		}
	return 0;
}

/* See tree-pass.h for a list and desctiptions for the fields of this struct */
static struct gimple_opt_pass speller_pass;

/* Return 0 on success or error code on failure */
int plugin_init(struct plugin_name_args *info, /* Argument infor */ struct plugin_gcc_version *ver) /* Version of GCC */ {
	struct register_pass_info pass;

	if (strncmp(ver->basever, speller_ver.basever, strlen(VER)))
		return -1; /* Incorrect version of gcc */

	speller_pass.pass.type=GIMPLE_PASS;
	speller_pass.pass.name="speller";
	speller_pass.pass.gate=speller_gate;
	speller_pass.pass.execute=speller_exec;
	pass.pass = &speller_pass.pass;
	pass.reference_pass_name = "ssa";
	pass.ref_pass_instance_number = 1;
	pass.pos_op = PASS_POS_INSERT_AFTER;

	/* Tell gcc we want to be called after the first SSA pass */
	register_callback("speller", PLUGIN_PASS_MANAGER_SETUP, NULL, &pass);

	#ifdef DO_INFO
	register_callback("speller", PLUGIN_INFO, NULL, &speller_info);
	#endif // DO_INFO

	/* Initilize our spell checker */
	if (!(speller_g = init_spellchecker()))
		return -1;
	return 0;
}
