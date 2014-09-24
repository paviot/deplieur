with
  FRAME_KIT_GENERIC_MAIN,
  FK_API_DEFAULT_INTERRUPT_HANDLER,
  Select_Unfolded_Classes_Body;
use
  FK_API_DEFAULT_INTERRUPT_HANDLER;

procedure Select_Unfolded_Classes is
   new Frame_Kit_Generic_Main
  (TRACE_FILE_NAME => "select_unfolded_classes",
   IS_STANDALONE_SUPPORTED => True,
   MAIN_ALGORITHM => Select_Unfolded_Classes_Body,
   TOOL_NAME => "select_unfolded_classes",
   TOOL_VERSION => "1.0",
   TOOL_COPYRIGHT => "Alban Linard (LIP6-SRC), 2005",
   KIT_NAME => "col2ordequivDDD");
