#include <iostream.h>
#include <unistd.h>
#include "dta_to_json.h"

int main (int argc, const char * argv []) {
  char * dot_json = "json";
  char * dest, * dest2, * dest3;
  dest = dest2 = dest3 = NULL;

  if (! (argc == 2 || argc == 3 || argc == 5) ) {
	cout << "Usage: dta2json <src> <dest>\n\t dta2json <src>" << endl;
	cout << "    OR dta2json <src> <hit-based> <time-based> <waveform>" << endl;
	return 0;
  }

  if (argc == 2) {
	size_t len = strlen (argv [1]);
	dest = (char *) malloc (len + 1);
	// Copy name of first file except for extension, append extension
	memcpy (dest, argv [1], len - 3);
	memcpy (&dest [len - 3], dot_json, 4);
	dest [len] = NULL;
  } else if (argc == 3) {
	dest = (char *) argv [2];
  } else {
	dest = (char *) argv [2];
	dest2 = (char *) argv [3];
	dest3 = (char *) argv [4];
  }

  FILE* dta_file = fopen (argv [1], "rb");
  FILE* output_file1 = fopen (dest, "w");
  FILE* output_file2, * output_file3;
  if (argc == 5) {
	output_file2 = fopen (dest2, "w");
	output_file3 = fopen (dest3, "w");
  } else { output_file2 = output_file3 = output_file1; }

  m1_control message1_state;
  m2_control message2_state;
  m173_control message173_state;
  m110_data p_info;
  int num_pp_segs;

  yajl_gen_config conf = {1, "    "};
  yajl_gen g = yajl_gen_alloc (&conf, NULL);
  yajl_gen g2, g3;
  if (argc == 5) {
	g2 = yajl_gen_alloc (&conf, NULL);
	g3 = yajl_gen_alloc (&conf, NULL);
  } else { g2 = g3 = g; }

  yajl_gen_array_open (g);
  if (argc == 5) { yajl_gen_array_open (g2); yajl_gen_array_open (g3); }

  message2_state.partial_power_segs_p = message1_state.partial_power_segs_p = &num_pp_segs;
  message1_state.json_handle = g;
  message2_state.json_handle = g2;
  message173_state.json_handle = g3;
  message173_state.output_handle = output_file3;
  message2_state.output_handle = output_file2;
  message1_state.output_handle = output_file1;
  message1_state.parametric_info = &p_info;

  json_ctx_init ();
  json_handlers_init ();

  json_ctx [1] = &message1_state;
  json_ctx [2] = &message2_state;
  json_ctx [5] = &message1_state;
  json_ctx [6] = &message2_state;
  json_ctx [23] = &message173_state;
  json_ctx [24] = &message173_state;
  json_ctx [26] = &message173_state;
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
  if (argc == 5) { yajl_gen_array_close (g2); yajl_gen_array_close (g3); }
  
  const unsigned char * buf;
  unsigned int len;
  yajl_gen_get_buf (g, &buf, &len);
  fwrite(buf, 1, len, output_file1);
  yajl_gen_clear (g);
  yajl_gen_free (g);
  if (argc == 5) {
	yajl_gen_get_buf (g2, &buf, &len);
	fwrite(buf, 1, len, output_file2);
	yajl_gen_clear (g2);
	yajl_gen_free (g2);

	yajl_gen_get_buf (g3, &buf, &len);
	fwrite(buf, 1, len, output_file3);
	yajl_gen_clear (g3);
	yajl_gen_free (g3);
  }

  cout << "Conversion Complete." << endl;

  return 0;
}
