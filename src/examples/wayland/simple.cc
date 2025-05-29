/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <err_utils.h>
#include <wayland-client.h>
#include <stdio.h>
#include <string.h>

/*
 * Simple wayland exapmle
 *
 * EXTRA_COMPILE_FLAGS=-std=c++20
 * EXTRA_LINK_FLAGS_AFTER=-lwayland-client
 */

struct wl_shell *shell = NULL;
struct wl_shell_surface *shell_surface = NULL;
struct wl_surface *surface = NULL;

static void registry_global(void* data __attribute__((unused)), struct wl_registry *registry,
	uint32_t name, const char *interface,
	uint32_t version __attribute__((unused))) {
	if (strcmp(interface, "wl_shell") == 0) {
		shell = (struct wl_shell *) wl_registry_bind(registry, name, &wl_shell_interface, 1);
	}
}

static void registry_global_remove(void* data __attribute__((unused)),
	struct wl_registry *registry __attribute__((unused)),
	uint32_t name __attribute__((unused))) {
	// Do nothing
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

int main() {
	struct wl_display *display = (struct wl_display*)CHECK_NOT_NULL(wl_display_connect(NULL));
	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);
	if (shell == NULL) {
		fprintf(stderr, "Failed to get wl_shell global\n");
		wl_display_disconnect(display);
		return EXIT_FAILURE;
	}
	surface = wl_compositor_create_surface(NULL);
	shell_surface = wl_shell_get_shell_surface(shell, surface);
	wl_shell_surface_set_toplevel(shell_surface);
	wl_surface_commit(surface);
	while (wl_display_dispatch(display) != -1) {
		// Wait for events
	}
	wl_shell_surface_destroy(shell_surface);
	wl_surface_destroy(surface);
	wl_shell_destroy(shell);
	wl_registry_destroy(registry);
	wl_display_disconnect(display);
	return EXIT_SUCCESS;
}
