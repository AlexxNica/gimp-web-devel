#include <libgimp/gimp.h>

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);
static void blur  (GimpDrawable     *drawable);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    "plug-in-myblur1",
    "My blur 1 (slow)",
    "Blurs the image",
    "David Neary",
    "Copyright David Neary",
    "2004",
    "_My blur 1 (slow)",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-myblur1",
                             "<Image>/Filters/Blur");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;
  GimpDrawable     *drawable;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if
   * we are in NONINTERACTIVE mode
   */
  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
  drawable = gimp_drawable_get (param[2].data.d_drawable);

  gimp_progress_init ("My Blur...");

  /* Let's time blur
   *
   *   GTimer timer = g_timer_new time ();
   */

  blur (drawable);

  /*   g_print ("blur() took %g seconds.\n", g_timer_elapsed (timer));
   *   g_timer_destroy (timer);
   */

  gimp_displays_flush ();
  gimp_drawable_detach (drawable);
}

static void
blur (GimpDrawable *drawable)
{
  gint         i, j, k, channels;
  gint         x1, y1, x2, y2;
  GimpPixelRgn rgn_in, rgn_out;
  guchar       output[4];

  /* Gets upper left and lower right coordinates,
   * and layers number in the image */
  gimp_drawable_mask_bounds (drawable->drawable_id,
                             &x1, &y1,
                             &x2, &y2);
  channels = gimp_drawable_bpp (drawable->drawable_id);

  /* Initialises two PixelRgns, one to read original data,
   * and the other to write output data. That second one will
   * be merged at the end by the call to
   * gimp_drawable_merge_shadow() */
  gimp_pixel_rgn_init (&rgn_in,
                       drawable,
                       x1, y1,
                       x2 - x1, y2 - y1,
                       FALSE, FALSE);
  gimp_pixel_rgn_init (&rgn_out,
                       drawable,
                       x1, y1,
                       x2 - x1, y2 - y1,
                       TRUE, TRUE);

  for (i = x1; i < x2; i++)
    {
      for (j = y1; j < y2; j++)
        {
          guchar pixel[9][4];

          /* Get nine pixels */
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[0],
                                    MAX (i - 1, x1), MAX (j - 1, y1));
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[1],
                                    MAX (i - 1, x1), j);
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[2],
                                    MAX (i - 1, x1), MIN (j + 1, y2 - 1));

          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[3],
                                    i, MAX (j - 1, y1));
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[4],
                                    i, j);
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[5],
                                    i, MIN (j + 1, y2 - 1));

          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[6],
                                    MIN (i + 1, x2 - 1), MAX (j - 1, y1));
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[7],
                                    MIN (i + 1, x2 - 1), j);
          gimp_pixel_rgn_get_pixel (&rgn_in,
                                    pixel[8],
                                    MIN (i + 1, x2 - 1), MIN (j + 1, y2 - 1));

          /* For each layer, compute the average of the
           * nine */
          for (k = 0; k < channels; k++)
            {
              int tmp, sum = 0;
              for (tmp = 0; tmp < 9; tmp++)
                sum += pixel[tmp][k];
              output[k] = sum / 9;
            }

          gimp_pixel_rgn_set_pixel (&rgn_out,
                                    output,
                                    i, j);
        }

      if (i % 10 == 0)
        gimp_progress_update ((gdouble) (i - x1) / (gdouble) (x2 - x1));
    }

  /* Update the modified region */
  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id,
                        x1, y1,
                        x2 - x1, y2 - y1);
}

