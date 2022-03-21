package lang.smart.syncheck;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.ParseTreeWalker;
import lang.smart.syncheck.gen.SmartLexer;
import lang.smart.syncheck.gen.SmartListener;
import lang.smart.syncheck.gen.SmartParser;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {
    static String getExt(String name) {
        int pos = name.lastIndexOf('.');
        if (pos<0)
            return "";
        else
            return name.substring(pos);
    }

    static boolean filter_ch(File f) {
        String ext = getExt(f.getName());
        return ext.equals(".slan");
    }

    static List<File> listf(String directoryName) {
        File directory = new File(directoryName);

        List<File> resultList = new ArrayList<>();

        File[] fList = directory.listFiles();
        assert fList != null;
        for (File file : fList) {
            if (file.isFile()) {
                if (filter_ch(file))
                    resultList.add(file);
            } else if (file.isDirectory()) {
                resultList.addAll(listf(file.getAbsolutePath()));
            }
        }
        return resultList;
    }

    static void parse(String inputFileName) throws IOException {
        System.out.println(inputFileName);
        CharStream codePointCharStream = CharStreams.fromFileName(inputFileName);
        SmartLexer lexer = new SmartLexer(codePointCharStream);
        /*Token token;
        do {
            token = lexer.nextToken();
            System.out.printf("%s type=%d index=%d line=%d\n",token.getText(),token.getType(), token.getTokenIndex(), token.getLine() );
        } while (token.getType()!=-1);*/
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        SmartParser parser = new SmartParser(tokens);
        //parser.removeErrorListeners();
        //MyAntlrErrorListener errorListener = new MyAntlrErrorListener();
        //parser.addErrorListener(errorListener);
        SmartParser.CompilationUnitContext tree = parser.compilationUnit();
        //SmartListener extractor = new CListenerImpl();
        //ParseTreeWalker.DEFAULT.walk(extractor, tree);
    }

    static void testDir() throws IOException {
        List<File> files = listf("../samples");
        System.out.printf("%d files in directory",files.size());
        for (File file: files) {
            parse(file.getPath());
        }
    }

    static void testOne() throws IOException {
        parse("../samples/Tree.slan");
    }

    public static void main(String[] args) throws IOException {
        testDir();
    }
}
