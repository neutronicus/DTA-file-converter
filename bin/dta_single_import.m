function varargout = dta_single_import(varargin)
% DTA_SINGLE_IMPORT MATLAB code for dta_single_import.fig
%      DTA_SINGLE_IMPORT, by itself, creates a new DTA_SINGLE_IMPORT or raises the existing
%      singleton*.
%
%      H = DTA_SINGLE_IMPORT returns the handle to a new DTA_SINGLE_IMPORT or the handle to
%      the existing singleton*.
%
%      DTA_SINGLE_IMPORT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in DTA_SINGLE_IMPORT.M with the given input arguments.
%
%      DTA_SINGLE_IMPORT('Property','Value',...) creates a new DTA_SINGLE_IMPORT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before dta_single_import_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to dta_single_import_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help dta_single_import

% Last Modified by GUIDE v2.5 08-Feb-2011 14:36:52

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @dta_single_import_OpeningFcn, ...
                   'gui_OutputFcn',  @dta_single_import_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before dta_single_import is made visible.
function dta_single_import_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to dta_single_import (see VARARGIN)

% Choose default command line output for dta_single_import
handles.output = hObject;

% All checkboxes true by default.
set(handles.hit_based_checkbox, 'Value', 1);
set(handles.time_based_checkbox, 'Value', 1);
set(handles.waveform_checkbox, 'Value', 1);
set(handles.time_mark_checkbox, 'Value', 1);

handles.hit_based_option = 1;
handles.time_based_option = 1;
handles.waveform_option = 1;
handles.time_mark_option = 1;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes dta_single_import wait for user response (see UIRESUME)
uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = dta_single_import_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure


% --- Executes on button press in hit_based_checkbox.
function hit_based_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to hit_based_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of hit_based_checkbox
handles.hit_based_option = get (hObject, 'Value');
guidata (hObject, handles);

% --- Executes on button press in time_based_checkbox.
function time_based_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to time_based_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of time_based_checkbox
handles.time_based_option = get (hObject, 'Value');
guidata (hObject, handles);


% --- Executes on button press in waveform_checkbox.
function waveform_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to waveform_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of waveform_checkbox
handles.waveform_option = get (hObject, 'Value');
guidata (hObject, handles);

% --- Executes on button press in time_mark_checkbox.
function time_mark_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to time_mark_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of time_mark_checkbox
handles.time_mark_option = get (hObject, 'Value');
guidata (hObject, handles);

function hit_based_var_edit_Callback(hObject, eventdata, handles)
% hObject    handle to hit_based_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of hit_based_var_edit as text
%        str2double(get(hObject,'String')) returns contents of hit_based_var_edit as a double
handles.hit_based_var_name = get(hObject, 'String');
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function hit_based_var_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to hit_based_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function time_based_var_edit_Callback(hObject, eventdata, handles)
% hObject    handle to time_based_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of time_based_var_edit as text
%        str2double(get(hObject,'String')) returns contents of
%        time_based_var_edit as a double
handles.time_based_var_name = get(hObject, 'String');
guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function time_based_var_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to time_based_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function waveform_var_edit_Callback(hObject, eventdata, handles)
% hObject    handle to waveform_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of waveform_var_edit as text
%        str2double(get(hObject,'String')) returns contents of waveform_var_edit as a double
handles.waveform_var_name = get (hObject, 'String');
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function waveform_var_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to waveform_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function time_marks_var_edit_Callback(hObject, eventdata, handles)
% hObject    handle to time_marks_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of time_marks_var_edit as text
%        str2double(get(hObject,'String')) returns contents of time_marks_var_edit as a double
handles.time_marks_var_name = get(hObject, 'String');
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function time_marks_var_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to time_marks_var_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in file_browse_button.
function file_browse_button_Callback(hObject, eventdata, handles)
% hObject    handle to file_browse_button (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

[file_name file_path filter_index] = uigetfile('*.DTA', 'MultiSelect', ...
                                               'on');

handles.file_name = file_name;
handles.file_path = file_path;
if iscell (file_name)
  for n=1:length (file_name) 
    if n < length (file_name)
      file_name{i} = [file_path file_name(i) ', '];
    else
      file_name{i} = [file_path file_name (i)];
    end
  end
  set (handle.dta_file_edit, [file_name]);
else
  set (handles.dta_file_edit, 'String', [file_path file_name]);
end

guidata(hObject, handles);


function dta_file_edit_Callback(hObject, eventdata, handles)
% hObject    handle to dta_file_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of dta_file_edit as text
%        str2double(get(hObject,'String')) returns contents of dta_file_edit as a double
file_name = get(hObject, 'String');
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function dta_file_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to dta_file_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in import_button.
function import_button_Callback(hObject, eventdata, handles)
% hObject    handle to import_button (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%close(handles.figure1);
varargout{1} = handles.output;

if ischar (handles.file_name)
  import_dta_private(get(handles.dta_file_edit, 'String'),...
                     {handles.hit_based_var_name
                      handles.time_based_var_name
                      handles.waveform_var_name
                      handles.time_marks_var_edit},...
                     logical([handles.hit_based_option
                      handles.time_based_option
                      handles.waveform_option
                      handles.time_mark_option]),...
                     'base');
elseif iscell (handles.file_name)
  % Pass it off to parallel computing toolbox if there are multiple files
  [a, b] =  import_multiple_dta (file_name);
  assignin ('base', 'a', a);
  assignin ('base', 'b', b);
end

guidata(hObject, handles);


% --- Executes on button press in cancel_button.
function cancel_button_Callback(hObject, eventdata, handles)
% hObject    handle to cancel_button (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close all;