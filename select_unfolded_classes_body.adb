with
  Fk_Api_Environment_Communication,
  Fk_Api_User_Interface,
  Fk_Api_Data_Management,
  Utilitaires_Pour_Fichiers_Texte,
  Presentation,
  Glst_Chaine_Variables,
  Glst_Integer,
  Fk_Strings;
use
  Fk_Api_Environment_Communication,
  Fk_Api_User_Interface,
  Fk_Api_Data_Management,
  Utilitaires_Pour_Fichiers_Texte,
  Presentation,
  Glst_Chaine_Variables,
  Glst_Integer,
  Fk_Strings;

procedure Select_Unfolded_Classes_Body is
   Input_File : VSTRING;
   Output_File : VSTRING;
   Default_Input_File : VSTRING := TO_VSTRING(".pnet_classes");
   Default_Output_File : VSTRING := TO_VSTRING(".pnet_unfolded_classes");
   Classes : LISTE_CHAINE_VARIABLE;
   Selected_Classes : LISTE_INTEGER;
begin
   if FK_IS_MODE_STANDALONE then
      -- Get input and output file names.
      if FK_ARG_COUNT /= 3 then
         raise FK_PROCESSED_WITH_PROBLEM;
      else
         Input_File := TO_VSTRING(FK_ARG_VALUE(1));
         Output_File := TO_VSTRING(FK_ARG_VALUE(2));
      end if;
   else
      -- Open default file, read it and write to default file later.
      Input_File := Default_Input_File;
      Output_File := Default_Output_File;
   end if;
   declare
      File : FIC_TXT_LECT;
   begin
      File := OUVRIR_FICHIER
        (NOM => FK_GET_FILE_LINK
         (F_TYPE => KFK_FILE_DEFAULT,
          MAIN_INFO => TO_STRING(Input_File)),
         EN_LIGNE => False);
      while not FIN_DE_FICHIER(ID => File) loop
         declare
            Name : VSTRING;
         begin
            Name := RECUPERER_LIGNE
              (FICH => File);
            if (Name /= EMPTY_VSTRING) then
               Classes := AJOUTER_QUEUE
                 (OBJ => Name,
                  LST => Classes);
            end if;
         end;
      end loop;
   exception
      when others =>
         FERME_FICHIER(ID => File);
   end;
   FK_SELECT_ITEMS_IN_LIST
     (MSG => "Please, select color classes to unfold :",
      LIST => Classes,
      BUTTON1 => "OK",
      BUTTON2 => "CANCEL",
      TITLE => "Classes selection",
      RESULT => Selected_Classes);
   declare
      File : FIC_TXT;
      Margin : IDENTIFICATEUR_MARGE := CREER_MARGE;
   begin
      CREATE (FILE => File,
              NAME => FK_GET_FILE_LINK
              (F_TYPE => KFK_FILE_DEFAULT,
               MAIN_INFO => TO_STRING(Output_File)));
      Selected_Classes := PREMIER_ELEMENT(LST => Selected_Classes);
      while True loop
         declare
            Name : VSTRING;
         begin
            Name := CONTENU(LST => NIEME_ELEMENT
                            (LST => Classes,
                             N => CONTENU
                             (LST => Selected_Classes)));
            ECRIRE (FIC => File,
                    MARGE => Margin,
                    TEXTE => TO_STRING
                    (Name),
                    RETOUR_LIGNE => True);
            Selected_Classes := SUIVANT(ELEM => Selected_Classes);
         end;
      end loop;
   exception
     when others =>
        CLOSE (FIC => File);
   end;
end Select_Unfolded_Classes_Body;
