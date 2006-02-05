#include <libgimp/gimp.h>

static void query       (void);
static void run         (const gchar      *name,
                         gint              nparams,
                         const GimpParam  *param,
                         gint             *nreturn_vals,
                         GimpParam       **return_vals);

static void blur        (GimpDrawable     *drawable);

static void init_mem    (guchar         ***row,
                         guchar          **outrow,
                         gint              num_bytes);
static void process_row (guchar          **row,
                         guchar           *outrow,
                         gint              x1,
                         gint              y1,
                         gint              width,
                         gint              height,
                         gint              channels,
                         gint              i);
static void shuffle     (GimpPixelRgn     *rgn_in,
                         guchar          **row,
                         gint              x1,
                         gint              y1,
                         gint              width,
                         gint              height,
                         gint              ypos);

/* The radius is still a constant, we'll change that when the
 * graphical interface will be built. */
static gint radius = 3;

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
    "plug-in-myblur3",
    "My blur 3 (tiled)",
    "Blurs the image",
    "David Neary",
    "Copyright David Neary",
    "2004",
    "_My blur 3 (tiled)",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-myblur3",
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
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
  drawable = gimp_drawable_get (param[2].data.d_drawable);

  blur (drawable);

  gimp_displays_flush ();
  gimp_drawable_detach (drawable);

  return;
}

static void
blur (GimpDrawable *drawable)
{
  gint         i, ii, channels;
  gint         x1, y1, x2, y2;
  GimpPixelRgn rgn_in, rgn_out;
  guchar     **row;
  guchar      *outrow;
  gint         width, height;

  gimp_progress_init ("My Blur...");

  /* Gets upper left and lower right coordinates,
   * and layers number in the image */
  gimp_drawable_mask_bounds (drawable->drawable_id,
                             &x1, &y1,
                             &x2, &y2);
  width  = x2 - x1;
  height = y2 - y1;

  channels = gimp_drawable_bpp (drawable->drawable_id);

  /* Allocate a big enough tile cache */
  gimp_tile_cache_ntiles (2 * (drawable->width / gimp_tile_width () + 1));

  /* Initialises two PixelRgns, one to read original data,
   * and the other to write output data. That second one will
   * be merged at the end by the call to
   * gimp_drawable_merge_shadow() */
  gimp_pixel_rgn_init (&rgn_in,
                       drawable,
                       x1, y1,
                       width, height,
                       FALSE, FALSE);
  gimp_pixel_rgn_init (&rgn_out,
                       drawable,
                       x1, y1,
                       width, height,
                       TRUE, TRUE);

  /* Allocate memory for input and output tile rows */
  init_mem (&row, &outrow, width * channels);

  for (ii = -radius; ii <= radius; ii++)
    {
      gimp_pixel_rgn_get_row (&rgn_in,
                              row[radius + ii],
                              x1, y1 + CLAMP (ii, 0, height - 1),
                              width);
    }

  for (i = 0; i < height; i++)
    {
      /* To be done for each tile row */
      process_row (row,
                   outrow,
                   x1, y1,
                   width, height,
                   channels,
                   i);
      gimp_pixel_rgn_set_row (&rgn_out,
                              outrow,
                              x1, i + y1,
                              width);
      /* shift tile rows to insert the new one at the end */
      shuffle (&rgn_in,
               row,
               x1, y1,
               width, height,
               i);
      if (i % 10 == 0)
        gimp_progress_update ((gdouble) i / (gdouble) height);
    }

  /* We could also put that in a separate function but it's
   * rather simple */
  for (ii = 0; ii < 2 * radius + 1; ii++)
    g_free (row[ii]);

  g_free (row);
  g_free (outrow);

  /*  Update the modified region */
  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id,
                        x1, y1,
                        width, height);
}

static void
init_mem (guchar ***row,
          guchar  **outrow,
          gint      num_bytes)
{
  gint i;

  /* Allocate enough memory for row and outrow */
  *row = g_new (guchar *, (2 * radius + 1));

  for (i = -radius; i <= radius; i++)
    (*row)[i + radius] = g_new (guchar, num_bytes);

  *outrow = g_new (guchar, num_bytes);
}

static void
process_row (guchar **row,
             guchar  *outrow,
             gint     x1,
             gint     y1,
             gint     width,
             gint     height,
             gint     channels,
             gint     i)
{
  gint j;

  for (j = 0; j < width; j++)
    {
      gint k, ii, jj;
      gint left = (j - radius),
           right = (j + radius);

      /* For each layer, compute the average of the
       * (2r+1)x(2r+1) pixels */
      for (k = 0; k < channels; k++)
        {
          gint sum = 0;

          for (ii = 0; ii < 2 * radius + 1; ii++)
            for (jj = left; jj <= right; jj++)
              sum += row[ii][channels * CLAMP (jj, 0, width - 1) + k];

          outrow[channels * j + k] =
            sum / (4 * radius * radius + 4 * radius + 1);
        }
    }
}

static void
shuffle (GimpPixelRgn *rgn_in,
         guchar      **row,
         gint          x1,
         gint          y1,
         gint          width,
         gint          height,
         gint          ypos)
{
  gint    i;
  guchar *tmp_row;

  /* Get tile row (i + radius + 1) into row[0] */
  gimp_pixel_rgn_get_row (rgn_in,
                          row[0],
                          x1, MIN (ypos + radius + y1, y1 + height - 1),
                          width);

  /* Permute row[i] with row[i-1] and row[0] with row[2r] */
  tmp_row = row[0];
  for (i = 1; i < 2 * radius + 1; i++)
    row[i - 1] = row[i];
  row[2 * radius] = tmp_row;
}

