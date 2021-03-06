
SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  =
EXES                  = 7k2



### Common settings

CEXTRA                = -g -O2 -Wall -m32
CXXEXTRA              = -g -O2 -Wall -m32
RCEXTRA               =
DEFINES               =
INCLUDE_PATH          = -I. \
			-I. \
			-Iinclude \
			-I.
DLL_PATH              =
DLL_IMPORTS           =
LIBRARY_PATH          =
LIBRARIES             =

7k2_LDFLAGS   = -m32
7k2_ARFLAGS   =
7k2_DLL_PATH  =
7k2_DLLS      =
7k2_LIBRARY_PATH=
7k2_LIBRARIES = jpeg SDL SDL_mixer



### 7k2 sources and settings

7k2_MODULE    = 7k2
7k2_C_SRCS    =
7k2_CXX_SRCS  = src/platform.cpp \
			src/ounadraw.cpp \
			src/of_inni.cpp \
			src/of_traii.cpp \
			src/oai_act.cpp \
			src/ou_god2.cpp \
			src/of_resei.cpp \
			src/ow_fire.cpp \
			src/of_magii.cpp \
			src/oun_spab.cpp \
			src/olog.cpp \
			src/ocnation.cpp \
			src/oc_pld1.cpp \
			src/osedit_r.cpp \
			src/ou_caras.cpp \
			src/osedit_o.cpp \
			src/of_resea.cpp \
			src/ospinner.cpp \
			src/opreuse.cpp \
			src/ocampgn2.cpp \
			src/olocate.cpp \
			src/ojpeg.cpp \
			src/ogammain.cpp \
			src/oc_east7.cpp \
			src/oc_east4.cpp \
			src/otowna.cpp \
			src/osedit_p.cpp \
			src/oc_east2.cpp \
			src/osprtres.cpp \
			src/oc_east1.cpp \
			src/ovbrowif.cpp \
			src/ou_mons.cpp \
			src/or_tech.cpp \
			src/of_base.cpp \
			src/otornado.cpp \
			src/ofirmai.cpp \
			src/otrainee.cpp \
			src/onationa.cpp \
			src/of_mtrn.cpp \
			src/ohillres.cpp \
			src/obullet.cpp \
			src/ot_unit.cpp \
			src/oc_eas18.cpp \
			src/oc_pla2.cpp \
			src/oflc.cpp \
			src/of_off2i.cpp \
			src/oitem.cpp \
			src/otwalres.cpp \
			src/or_town.cpp \
			src/ogfile.cpp \
			src/ot_camp.cpp \
			src/ovbrowse.cpp \
			src/oc_eas24.cpp \
			src/oslidcus.cpp \
			src/ospin_s.cpp \
			src/ot_sedit.cpp \
			src/otown.cpp \
			src/oun_drw.cpp \
			src/of_off.cpp \
			src/ogfile3.cpp \
			src/of_spyi.cpp \
			src/ot_gmenu.cpp \
			src/otownif.cpp \
			src/oplace.cpp \
			src/ofirmdie.cpp \
			src/ot_endc.cpp \
			src/of_camp.cpp \
			src/oremote2.cpp \
			src/oplasma.cpp \
			src/omisc.cpp \
			src/ounitb.cpp \
			src/of_mfori.cpp \
			src/oingmenu.cpp \
			src/oc_east6.cpp \
			src/of_mfork.cpp \
			src/of_minei.cpp \
			src/oc_eas14.cpp \
			src/ofirmres.cpp \
			src/ofile.cpp \
			src/ogfile2.cpp \
			src/of_alch.cpp \
			src/otownres.cpp \
			src/opfind.cpp \
			src/ofont.cpp \
			src/of_camps.cpp \
			src/obitmapw.cpp \
			src/ositedrw.cpp \
			src/otownat.cpp \
			src/of_alchi.cpp \
			src/oai_spy.cpp \
			src/oai_dipl.cpp \
			src/oc_pla7.cpp \
			src/or_trade.cpp \
			src/oun_mode.cpp \
			src/ose.cpp \
			src/ob_homin.cpp \
			src/omem.cpp \
			src/ou_wagon.cpp \
			src/ofirm.cpp \
			src/orle.cpp \
			src/oc_pla3.cpp \
			src/obutton.cpp \
			src/ogamhall.cpp \
			src/omouse.cpp \
			src/oskill.cpp \
			src/oai_act2.cpp \
			src/oc_eas21.cpp \
			src/oresdb.cpp \
			src/or_news.cpp \
			src/oc_pld2.cpp \
			src/oc_eas12.cpp \
			src/osprite.cpp \
			src/onews.cpp \
			src/orawres.cpp \
			src/oc_eas28.cpp \
			src/ovgalock.cpp \
			src/of_train.cpp \
			src/osprite2.cpp \
			src/oimgres.cpp \
			src/oc_eas11.cpp \
			src/omusic.cpp \
			src/of_forti.cpp \
			src/ou_caraa.cpp \
			src/oai_defe.cpp \
			src/of_inn.cpp \
			src/oc_eas20.cpp \
			src/of_wara.cpp \
			src/osysfile.cpp \
			src/olocbmp.cpp \
			src/osysinit.cpp \
			src/ovgabuf2.cpp \
			src/otechres.cpp \
			src/obitmap.cpp \
			src/oc_east3.cpp \
			src/oworld.cpp \
			src/of_forta.cpp \
			src/oai_quer.cpp \
			src/otalkmsg.cpp \
			src/oc_plc3.cpp \
			src/ogamend.cpp \
			src/oc_eas27.cpp \
			src/ooptmenu.cpp \
			src/oseditor.cpp \
			src/otownbld.cpp \
			src/ocoltbl.cpp \
			src/oun_ai.cpp \
			src/of_spy.cpp \
			src/oc_plc2.cpp \
			src/oun_ind.cpp \
			src/otowndrw.cpp \
			src/orock.cpp \
			src/omousecr.cpp \
			src/or_rank.cpp \
			src/omagic.cpp \
			src/of_worki.cpp \
			src/oremote.cpp \
			src/oprofile.cpp \
			src/of_mfort.cpp \
			src/oun_item.cpp \
			src/oprofil2.cpp \
			src/of_speca.cpp \
			src/of_work.cpp \
			src/oget.cpp \
			src/of_wari.cpp \
			src/oc_east9.cpp \
			src/oc_eas19.cpp \
			src/obutt3d.cpp \
			src/or_eco.cpp \
			src/ovga.cpp \
			src/obaseobj.cpp \
			src/ospya.cpp \
			src/ovqueue.cpp \
			src/osnowres.cpp \
			src/ospritea.cpp \
			src/ocampgn4.cpp \
			src/am.cpp \
			src/ogfilea.cpp \
			src/otownlnk.cpp \
			src/of_off2.cpp \
			src/ou_god.cpp \
			src/oc_eas26.cpp \
			src/oc_eas23.cpp \
			src/oimmplay.cpp \
			src/of_campi.cpp \
			src/ob_prog.cpp \
			src/obulleta.cpp \
			src/oremotem.cpp \
			src/of_laira.cpp \
			src/oai_gran.cpp \
			src/osite.cpp \
			src/of_spec.cpp \
			src/ogamcamp.cpp \
			src/ohelp.cpp \
			src/oc_east8.cpp \
			src/oeffect.cpp \
			src/oc_pld5.cpp \
			src/oitemres.cpp \
			src/oplant.cpp \
			src/ot_news.cpp \
			src/oai_main.cpp \
			src/osnow2.cpp \
			src/ogamsing.cpp \
			src/of_facta.cpp \
			src/omatrix.cpp \
			src/of_lishi.cpp \
			src/ou_carat.cpp \
			src/oherores.cpp \
			src/of_incui.cpp \
			src/oconfig.cpp \
			src/ogenmap2.cpp \
			src/ob_flame.cpp \
			src/of_lara2.cpp \
			src/ou_cart.cpp \
			src/ovbrowi2.cpp \
			src/ofirma.cpp \
			src/ow_trv.cpp \
			src/of_rese.cpp \
			src/oc_eas17.cpp \
			src/of_magi.cpp \
			src/oun_atk.cpp \
			src/olinear.cpp \
			src/owallres.cpp \
			src/of_incua.cpp \
			src/of_inna.cpp \
			src/oun_proc.cpp \
			src/ot_basic.cpp \
			src/oc_plot.cpp \
			src/ogame.cpp \
			src/of_worka.cpp \
			src/ob_proj.cpp \
			src/ogameset.cpp \
			src/ot_talk.cpp \
			src/of_marki.cpp \
			src/oc_eas13.cpp \
			src/oinfo.cpp \
			src/ogenmap.cpp \
			src/osysview.cpp \
			src/oun_atk2.cpp \
			src/ow_wall.cpp \
			src/oanline.cpp \
			src/omp_crc.cpp \
			src/of_campu.cpp \
			src/oworld_m.cpp \
			src/obox.cpp \
			src/orebel.cpp \
			src/ot_reps.cpp \
			src/ostr.cpp \
			src/ocrc_sto.cpp \
			src/of_animi.cpp \
			src/otransl.cpp \
			src/ogamecon.cpp \
			src/otutor.cpp \
			src/oresx.cpp \
			src/oc_eastp.cpp \
			src/ofirmat.cpp \
			src/oattack.cpp \
			src/oai_info.cpp \
			src/of_alcha.cpp \
			src/oai_mili.cpp \
			src/ores.cpp \
			src/oc_pla4.cpp \
			src/oc_eas25.cpp \
			src/oscroll.cpp \
			src/ofirmif.cpp \
			src/oevent.cpp \
			src/obattle.cpp \
			src/or_nat.cpp \
			src/of_magia.cpp \
			src/ostate.cpp \
			src/of_mtrna.cpp \
			src/ospy2.cpp \
			src/of_lish.cpp \
			src/ot_item.cpp \
			src/ovolume.cpp \
			src/of_anim.cpp \
			src/of_war.cpp \
			src/onewseng.cpp \
			src/oc_pla5.cpp \
			src/owarpt.cpp \
			src/olightn.cpp \
			src/odplay.cpp \
			src/of_incu.cpp \
			src/oc_pld4.cpp \
			src/odir.cpp \
			src/oblob.cpp \
			src/olonglog.cpp \
			src/osnowg.cpp \
			src/of_campb.cpp \
			src/oc_east5.cpp \
			src/ogenhill.cpp \
			src/ot_firm.cpp \
			src/oexpmask.cpp \
			src/ounitres.cpp \
			src/otalkeng.cpp \
			src/oun_if.cpp \
			src/oregion.cpp \
			src/orockres.cpp \
			src/odynarrb.cpp \
			src/of_offi.cpp \
			src/oc_east.cpp \
			src/ocampgn.cpp \
			src/odate.cpp \
			src/ou_cara.cpp \
			src/ospy.cpp \
			src/oremoteq.cpp \
			src/oc_pld3.cpp \
			src/or_spy.cpp \
			src/oc_eas10.cpp \
			src/otowndef.cpp \
			src/ogeta.cpp \
			src/osedit_h.cpp \
			src/olightn2.cpp \
			src/ow_rock.cpp \
			src/obuttcus.cpp \
			src/owaypnt.cpp \
			src/oun_info.cpp \
			src/oc_eas22.cpp \
			src/oai_capt.cpp \
			src/oformat.cpp \
			src/oai_attk.cpp \
			src/osysproc.cpp \
			src/on_human.cpp \
			src/oc_plc1.cpp \
			src/otutor3.cpp \
			src/ot_tutor.cpp \
			src/oblob2.cpp \
			src/osedit_v.cpp \
			src/omonsres.cpp \
			src/osedit_m.cpp \
			src/ogamcred.cpp \
			src/oai_cap2.cpp \
			src/ogamemp.cpp \
			src/oworld_z.cpp \
			src/otownwal.cpp \
			src/of_traia.cpp \
			src/of_marka.cpp \
			src/oc_easte.cpp \
			src/of_mark.cpp \
			src/orain2.cpp \
			src/or_mil.cpp \
			src/ovgabuf.cpp \
			src/ogamsce2.cpp \
			src/oseres.cpp \
			src/ounita.cpp \
			src/opower2.cpp \
			src/of_lairi.cpp \
			src/otownpop.cpp \
			src/of_minea.cpp \
			src/opower.cpp \
			src/oerror.cpp \
			src/ofirmdrw.cpp \
			src/oterrain.cpp \
			src/oerrctrl.cpp \
			src/oteam.cpp \
			src/osoldier.cpp \
			src/ogamscen.cpp \
			src/otips.cpp \
			src/oc_pla1.cpp \
			src/omodeid.cpp \
			src/otalkres.cpp \
			src/ofiletxt.cpp \
			src/of_speci.cpp \
			src/otownind.cpp \
			src/oraceres.cpp \
			src/oai_unit.cpp \
			src/of_basea.cpp \
			src/osysdet.cpp \
			src/oun_ord.cpp \
			src/obaseoba.cpp \
			src/otxtres.cpp \
			src/osedit_e.cpp \
			src/oai_talk.cpp \
			src/onews2.cpp \
			src/oblob2w.cpp \
			src/oun_ai2.cpp \
			src/of_lair.cpp \
			src/oun_act.cpp \
			src/of_spya.cpp \
			src/of_mtrni.cpp \
			src/orain1.cpp \
			src/of_fact.cpp \
			src/oc_pla6.cpp \
			src/ocampgn3.cpp \
			src/onationb.cpp \
			src/oc_eas16.cpp \
			src/oweather.cpp \
			src/odb.cpp \
			src/osnow1.cpp \
			src/oun_init.cpp \
			src/ow_plant.cpp \
			src/osysdisp.cpp \
			src/oc_eas15.cpp \
			src/of_monsi.cpp \
			src/oun_grp.cpp \
			src/oflame.cpp \
			src/of_fort.cpp \
			src/of_basew.cpp \
			src/ow_sound.cpp \
			src/or_ai.cpp \
			src/oun_hero.cpp \
			src/of_facti.cpp \
			src/oregions.cpp \
			src/oai_buil.cpp \
			src/of_mine.cpp \
			src/on_mons.cpp \
			src/oserial.cpp \
			src/ovga2.cpp \
			src/osedit_f.cpp \
			src/of_anima.cpp \
			src/odynarr.cpp \
			src/oc_plc4.cpp \
			src/oai_eco.cpp \
			src/ogodres.cpp \
			src/oaudio.cpp \
			src/oai_seek.cpp \
			src/otownai.cpp \
			src/of_campa.cpp \
			src/ou_godi.cpp \
			src/ou_carai.cpp \
			src/of_basei.cpp \
			src/omatrix2.cpp \
			src/orain3.cpp \
			src/osfrmres.cpp \
			src/oend_con.cpp \
			src/ofirmlnk.cpp \
			src/CRC.cpp

			
7k2_OBJS      = $(7k2_C_SRCS:.c=.o) \
			$(7k2_CXX_SRCS:.cpp=.o)

### Global source lists

C_SRCS                = $(7k2_C_SRCS)
CXX_SRCS              = $(7k2_CXX_SRCS)


### Tools

CC = gcc
CXX = g++
AR = ar r

### Generic targets

all: $(SUBDIRS) $(DLLS:%=%.so) $(LIBS) $(EXES)

### Build rules

.PHONY: all clean dummy

$(SUBDIRS): dummy
	@cd $@ && $(MAKE)

# Implicit rules

.SUFFIXES: .cpp .cxx .rc .res
DEFINCL = $(INCLUDE_PATH) $(DEFINES) $(OPTIONS)

.c.o:
	$(CC) -c $(CFLAGS) $(CEXTRA) $(DEFINCL) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	$(RM) $(CLEAN_FILES) $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
	$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(7k2_MODULE): $(7k2_OBJS)
	$(CXX) $(7k2_LDFLAGS) -o $@ $(7k2_OBJS) $(7k2_LIBRARY_PATH) $(DEFLIB) $(7k2_DLLS:%=-l%) $(7k2_LIBRARIES:%=-l%)


