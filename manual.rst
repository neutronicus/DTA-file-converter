.DTA to MATLAB importer
================

Building
-------

Windows
~~~~~~~

The project repository contains a file ``dta_tool.vcproj`` that can be used
to build the project using Visual Studio on a Windows system.  Some values
for the project will need to be changed for individual systems.  In the following,
``$MATLABROOT`` refers to the directory on your system where your distribution
is installed, for example, ``C:\Program Files\MATLAB\R2010a``

- ``Project -> Properties -> Linker -> General -> Additional Library Directories``
   should contain ``$MATLABROOT\extern\lib\win[64|32]\microsoft``, where the
   choice between 64 and 32 depends on the target platform.
- ``Project -> Properties -> C/C++ -> General -> Additional Include Directories``
   should contain ``$MATLABROOT\extern\include``
- ``Project -> Properties -> Linker -> Output File`` must have a file extention
   ``.mexw32`` or ``.mexw64`` that matches the target platform.

Linux
~~~~

In the following, ``$PROJECTROOT`` refers to the directory containing the project.

There is a makefile included that will build the project, but it requires a file called
``Makefile.inc`` that is not included with the project by default as it contains
system-specific information.  This file must define the following macros:

-``CC``, the command to invoke your system's C compiler
-``CXX``, the command to invoke your system's C++ compiler
-``MATLABROOT``, the Matlab root directory
-``CXXFLAGS``, C++ compiler flags, ``-arch x86_64`` on my system
-``CFLAGS``, C compiler flags, should be same as above
-``MEXFLAGS``, flags for the mex script (can be blank)
-``MEXSUFFIX``, the platform-dependent suffix for Matlab mex files

After this file has been created, open a shell and ``cd`` into the directory
containing the repository, and then invoke ``make``.  The mex file should be
created in the directory ``$PROJECTROOT/bin``, along with several other
Matlab scripts.  Adding this directory to the Matlab path should allow you to
use the commands in Matlab.

Usage
-----

Description of Data Format
~~~~~~~~~~~~~~~~~~~~~~~~~~

Hit-based Data
^^^^^^^^^^^^^^

The hit-based data are imported as a struct indexed by channel.  The struct
array has as many members as channels containing at least one event.  Note
that, if the first several channels are inactive, the index into this array does
*not* correspond to the id's of the channels on the circuit board.

This structure always contains the following fields:

- ``channel_id``
- ``tot`` (time of test)
- ``parametrics``

And will contain the following fields if they have been requested:

- ``x`` (x axis for waveforms)
- ``waveform``

Each of these fields is *data per event* by *number of events*, so ``waveform``,
for example, might be 2048 x 135, if there were 135 events.  Any additional AE
characteristics reported in the file will also have fields in this struct.  The possible
additional fields are:

.. _names:

-``rise_time``
-``counts_to_peak``
-``total_counts``
-``energy_counts``
-``duration``
-``amplitude``
-``rms_voltage``
-``asl``
-``gain``
-``threshold``
-``pre_amp_current``
-``lost_hits``
-``avg_freq``
-``rms_voltage``
-``reverb_freq``
-``init_freq``
-``signal_strength``
-``abs_energy``
-``partial_power``
-``freq_centroid``
-``freq_peak``

These are also *data per event*  by *number of events*, although the only one of
these that contains more than one datum per event is ``partial_power``

Time-based Data
^^^^^^^^^^^^^^^

The time-based data is imported in a scalar struct, which has three fields

-``tot``
-``parametrics``
-``channels``

The first is 1 by *number of events*, and the second is *number of parametrics*
by *number of events*.  The ``channels`` field contains as many members as
there were channels registering at least one time-based event, and each of those
members is a struct corresponding to each AE characteristic reported in the .DTA
file.  These fields can have any of the names_ listed above.

Time Marks
^^^^^^^^^^

All of the time marks in a given .DTA file are simply imported as a vector.

Import a single .DTA file from the Matlab prompt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to import the contents of a single file from the Matlab prompt, use the
function ``import_dta``.  This command takes one parameter, a string containing
the full path of the file to import.  The function can be called either with three
output arguments::

   [hit_based time_based marks] = import_dta ('example.dta')

or with none::

   import_dta ('example.dta')

If there are three, the hit-based data is placed into the first, the time-based data
into the second, and the time mark vector into the third.  If there are none, the
hit-based data is placed into ``a``, the time-based into ``b``, and the time
marks into ``m``.

The matlab extension requires that the file import_dta.mex<SUF>, where
<SUF> is a platform-dependent suffix, be on the matlab path.  Furthermore,
in order to build it, all of the MATLAB and MEX-related macros in the
makefile must be changed to point at the appropriate locations on your
system.

Once it is built and on the path, it can be invoked at the MATLAB prompt
or within a script as follows:

[a, b, x] = import_dta ('example.DTA');

``a`` will hold a structure array corresponding the hit-based AE characteristic
data and the waveform data.  ``b`` will hold a structure array corresponding to
the time-based AE characteristic data and the parametric data.  ``x`` will hold
an ``n_channels`` x ``n_samples`` matrix consisting of the abscissa for the wave-
-form data from each channel.

Import Data Split Across Multiple DTA Files
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There is also an included m-file that implements a function for importing
.dta files in parallel.  It is invoked as follows:

[a, b] = import_multiple_dta ({'file1.DTA', 'file2.DTA', ...});

The argument to this function is a cell array containing
several filenames.  After the command's completion, the variables
``a`` and ``b`` contain structure arrays whose fields are
distributed arrays.  These distributed arrays contain the data from
each of the files in the input cell array, concatenated together in
the order in which the files appear in the input cell array.

If the data are small enough to fit in memory, any one of the fields
of the structure can be made to reside entirely in the current
workspace using the ``gather`` command provided by the parallel
computing toolbox, like so:

local_channel_1_tot = gather(distributed_data(1).tot);

Both of these commands also have an optional second argument, which
must be a 1 by 4 logical array.  Each spot in this array corresponds
to a different option:

1. Import hit-based data
2. Import time-based data
3. Import waveform data
4. Import time mark data

Tagging Data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The fields of a Matlab struct are not set in stone - new ones can be
assigned at will.  This can be used, along with Matlab's logical
indexing, to tag data of interest.  Assuming the existence of some
Boolean function ``classify_data`` which, given a vector of data, will
assign a value of ``true`` or ``false`` to every data point, one can
tag data using the following method:

hit_based(1).tag_name = classify_data(hit_based(1))

After which the data can be logically indexed like so:

filtered_data = hit_based(1).tot(hit_based(1).tag_name)

Note that ``tag_name`` here is an arbitrary identifier that can be
freely chosen by the user.  The one caveat is that one should avoid
choosing any of the reserved _names, since doing so will overwrite the
data with a boolean array.
