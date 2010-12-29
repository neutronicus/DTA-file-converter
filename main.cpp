#include <iostream.h>
#include "dta_to_json.h"

int main (int argc, const char * argv []) {
  char * dot_json = "json";
  char * dest = NULL;
  
  if (! (argc == 2 || argc == 3) ) {
	cout << "Usage: dta2json <src> <dest>\n\t dta2json <src>" << endl;
	return 0;
  }

  if (argc == 2) {
	size_t len = strlen (argv [1]);
	dest = (char *) malloc (len + 1);
	// Copy name of first file except for extension, append extension
	memcpy (dest, argv [1], len - 3);
	memcpy (&dest [len - 3], dot_json, 4);
	dest [len] = NULL;
  } else {
	dest = (char *) argv [2];
  }

  FILE* dta_file = fopen (argv [1], "rb");
  FILE* output_file = fopen (dest, "w");

  m1_control message1_state;
  m2_control message2_state;
  m173_control message173_state;
  m110_data p_info;
  int num_pp_segs;

  yajl_gen_config conf = {1, "    "};
  yajl_gen g = yajl_gen_alloc (&conf, NULL);

  yajl_gen_array_open (g);

  message2_state.partial_power_segs_p = message1_state.partial_power_segs_p = &num_pp_segs;
  message173_state.json_handle = message2_state.json_handle = message1_state.json_handle = g;
  message173_state.output_handle = message2_state.output_handle = message1_state.output_handle = output_file;
  message1_state.parametric_info = &p_info;

  json_ctx_init ();
  json_handlers_init ();

  json_ctx [1] = &message1_state;
  json_ctx [2] = &message2_state;
  json_ctx [5] = &message1_state;
  json_ctx [6] = &message2_state;
  json_ctx [42] = dta_file;
  json_ctx [109] = &num_pp_segs;
  json_ctx [110] = &message1_state;
  json_ctx [173] = &message173_state;

  parse_dta_file (json_handlers, json_ctx, dta_file);

  free (message1_state.characteristics);
  free (message2_state.characteristics);
  free (message2_state.parametrics);
  free (p_info.pids);

  yajl_gen_array_close (g);

  const unsigned char * buf;
  unsigned int len;
  yajl_gen_get_buf (g, &buf, &len);
  fwrite(buf, 1, len, output_file);
  yajl_gen_clear(g);
  yajl_gen_free (g);

  cout << "Conversion Complete." << endl;

  return 0;
}
